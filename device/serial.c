#include "serial.h"

/* -------------------------------- Static Variables */

static const u32 srlRccUart[4] = {0, RCC_APB2Periph_USART1, RCC_APB1Periph_USART2, RCC_APB1Periph_USART3};
static const u32 srlRccGpio[4] = {0, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB};

static const GPIO_TypeDef *srlGpioPort[4]  = {0, GPIOA, GPIOA, GPIOB};
static const USART_TypeDef *srlUartPort[4] = {0, USART1, USART2, USART3};
static const enum IRQn srlUartIRQn[4]      = {0, USART1_IRQn, USART2_IRQn, USART3_IRQn};

static const u16 srlGpioPinTx[4] = {0, GPIO_Pin_9, GPIO_Pin_2, GPIO_Pin_10};
static const u16 srlGpioPinRx[4] = {0, GPIO_Pin_10, GPIO_Pin_3, GPIO_Pin_11};

// 串口接收缓冲区
volatile u8 serialSignBuf[SERIAL_BUF_SIZE];
volatile u8 serialSignOk = 0;
volatile u8 serialPacketBuf[SERIAL_BUF_SIZE];
volatile u8 serialPacketOk = 0;
volatile u8 serialPIDBuf[SERIAL_BUFFER_SIZE];
volatile u8 serialPIDOk = 0;
volatile u8 serialCMDBuf[SERIAL_BUFFER_SIZE];
volatile u8 serialCMDOk = 0;

// u8 signXFlag = 0;
// u8 signDFlag = 0;

/* -------------------------------- Global Functions */

// u8 get_sign_x_flag(void)
// {
//     if (signXFlag) {
//         signXFlag = 0;
//         return 1; // 返回1表示有新标志
//     }
//     return 0; // 返回0表示没有新标志
// }
// 
// u8 get_sign_d_flag(void)
// {
//     if (signDFlag) {
//         signDFlag = 0;
//         return 1; // 返回1表示有新标志
//     }
//     return 0; // 返回0表示没有新标志
// }

/******************************************************************
 * \brief  串口初始化
 * \param  srlNum 串口号 x=1,2,3
 * \param  baudRate 波特率
 * \param  subPriority 响应优先级 x=0~3
 *
 * \note   中断优先级：NVIC 分组1 (抢占1, 响应0~7)
 */
void serial_init(u8 srlNum, u32 baudRate, u8 subPriority)
{
    /* 开启时钟 */
    if (srlNum == 1)
        RCC_APB2PeriphClockCmd(srlRccUart[srlNum], ENABLE); // USART1 在 APB2
    else
        RCC_APB1PeriphClockCmd(srlRccUart[srlNum], ENABLE); // USART2/3 在 APB1
    RCC_APB2PeriphClockCmd(srlRccGpio[srlNum], ENABLE);  // 开启GPIOx的时钟（所有 GPIO 都在 APB2）

    /* GPIO 初始化 */
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin         = srlGpioPinTx[srlNum]; // Tx 引脚
    GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;     // GPIO速度
    GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;      // 复用推挽输出
    GPIO_Init((GPIO_TypeDef *)srlGpioPort[srlNum], &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = srlGpioPinRx[srlNum];  // Rx 引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init((GPIO_TypeDef *)srlGpioPort[srlNum], &GPIO_InitStructure);

    /* USART 初始化 */
    USART_InitTypeDef USART_InitStructure         = {0};
    USART_InitStructure.USART_BaudRate            = baudRate;                       // 波特率
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;            // 8位数据位
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;               // 1位停止位
    USART_InitStructure.USART_Parity              = USART_Parity_No;                // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;  // 收发模式
    USART_Init((USART_TypeDef *)srlUartPort[srlNum], &USART_InitStructure);         // 将结构体变量交给USART_Init，配置USARTx

    /* 中断输出配置 */
    USART_ITConfig((USART_TypeDef *)srlUartPort[srlNum], USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /* NVIC中断分组 */
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 配置NVIC为分组1 (抢占0~1，响应0~7)

    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure                  = {0};
    NVIC_InitStructure.NVIC_IRQChannel                   = srlUartIRQn[srlNum]; // 选择配置NVIC的USART3线
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                   // 抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = subPriority;         // 响应优先级为设定值
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;              // 指定NVIC线路使能
    NVIC_Init(&NVIC_InitStructure);                                             // 将结构体变量交给NVIC_Init，配置NVIC外设

    /* USART使能 */
    USART_Cmd((USART_TypeDef *)srlUartPort[srlNum], ENABLE); // 使能USARTx，串口开始运行
}

/*







*/

/******************************************************************
 * \brief  串口发送单字节
 * \param  USARTx 对应的串口号
 * \param  Byte 要发送的字节
 */
void serial_send_byte(USART_TypeDef *USARTx, u8 Byte)
{
    USART_SendData(USARTx, Byte);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/******************************************************************
 * \brief  串口发送字符串
 * \param  USARTx 对应的串口号
 * \param  String 要发送的字符串
 */
void serial_send_string(USART_TypeDef *USARTx, u8 *String)
{
    u8 i;
    for (i = 0; String[i] != '\0'; i++)
        serial_send_byte(USARTx, String[i]);
}

/******************************************************************
 * \brief  串口格式化发送函数
 * \param  USARTx 对应的串口号
 * \param  format 格式化字符串
 * \param  ...    可变参数列表
 */
void serial_printf(USART_TypeDef *USARTx, u8 *format, ...)
{
    char String[128];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    serial_send_string(USARTx, String);
}

/*







*/
// 
// /******************************************************************
//  * \brief  处理 USART3 接收到的数据包内容
//  * \note   在主循环中调用，当 serialPacketOk 为 1 时
//  */
// void serial_process_sign(void)
// {
//     if (!serialSignOk)
//         return;
// 
//     u8 t = serialSignBuf[1];
// 
//     // pass
// 
//     serialSignOk = 0;
//     if (dataEngine == rawDate)
//         serial_printf(USART1, "3> Sign: %c\n", t);
// }
// 
// /******************************************************************
//  * \brief  处理 USART3 接收到的数据包内容
//  * \note   在主循环中调用，当 serialPacketOk 为 1 时
//  */
// void serial_process_packet(void)
// {
//     if (!serialPacketOk)
//         return;
// 
//     u16 diff = (serialPacketBuf[1] << 8) | serialPacketBuf[2];
// 
//     // 限幅
//     diff = (diff > 320) ? 320 : diff;
//     diff = (diff < 0) ? 0 : diff;
// 
//     // 更新 PID 误差
//     if (lineFollowFlag == 1)
//         pidErr = diff;
//     else
//         pidErr = 160;
// 
//     serialPacketOk = 0;
//     if (dataEngine == rawDate)
//         serial_printf(USART1, "3> Diff: %d, %d\n", 160, diff);
// }
// 
// /******************************************************************
//  * \brief  处理 USART1 接收到的数据包内容
//  * \note   在主循环中调用，当 serialPIDOk 为 1 时
//  */
// void serial_process_pid(void)
// {
//     if (!serialPIDOk)
//         return;
// 
//     u8 len = serialPIDBuf[0];
//     if (len < 4) { // 至少要有一个字节
//         serialPIDOk = 0;
//         return;
//     }
// 
//     // 数据实际从 serialPIDBuf[1] 开始
//     char type = serialPIDBuf[1];
//     u8 num    = serialPIDBuf[2] - '0';
//     float v   = strtof((char *)&serialPIDBuf[4], NULL); // 从第4个字节开始转换为浮点数
//     char dis[8];
// 
//     // e.g. {Pn 0.05}
// 
//     switch (type) {
//         case 'P':
//             pidValue[num].Kp = v;
//             break;
//         case 'I':
//             pidValue[num].Ki = v;
//             break;
//         case 'D':
//             pidValue[num].Kd = v;
//             break;
//         default:
//             break;
//     }
//     serialPIDOk = 0;
//     if (dataEngine == rawDate)
//         serial_printf(USART1, "1> %c %d %g\n", type, num, v);
// }
// 
// /******************************************************************
//  * \brief  处理 USART1 接收到的目标坐标数据包
//  * \note   在主循环中调用，当 serialCMDOk 为 3 时
//  *         支持命令：
//  *            (spd lll rrr) 设置左右轮目标速度
//  *            (auto n)      设置巡线开关，n=0 关闭，n=1 开启
//  *            (apul n)      设置巡线平均脉冲，n=脉冲值
//  *            (vofa)        切换数据引擎
//  */
// void serial_process_cmd(void)
// {
//     if (!serialCMDOk)
//         return;
// 
//     // (spd lll rrr) / (auto n) / (apul n) / (vofa)
//     if (serialCMDBuf[1] == 's') {
//         u8 *pEnd;
//         s16 l = (s16)strtof((char *)&serialCMDBuf[5], &pEnd);
//         s16 r = (s16)strtof(pEnd + 1, NULL); // strof() 会主动跳过空格，+1 可以省略
// 
//         lineFollowFlag = 2; // 脉冲开环调试
// 
//         tarSpdLt = l;
//         tarSpdRt = r;
//     } else if (serialCMDBuf[1] == 'a') {
//         s16 n = (s16)strtof((char *)&serialCMDBuf[6], NULL);
//         if (serialCMDBuf[2] == 'u') {
//             if (n)
//                 lineFollowFlag = 1;
//             else
//                 lineFollowFlag = 0;
//         } else if (serialCMDBuf[2] == 'p') {
//             pulseAvg = (s16)n;
//         }
//     } else if (serialCMDBuf[1] == 'v') {
//         dataEngine = (dataEngine == rawDate) ? fireWater : rawDate;
//     }
// 
//     serialCMDOk = 0;
//     if (dataEngine == rawDate)
//         serial_printf(USART1, "1> CMD: %s\n", &serialCMDBuf[1]);
// }

/*







*/
// 
// /******************************************************************
//  * \brief  while 型串口等待机
//  * \param  flagString* 要发送的标志字符串
//  * \param  getFlagFun* 获取标志位的函数指针
//  *
//  * \note 该函数用于阻塞等待，直到获取到标志位为止
//  */
// void serial_wait_while(u8 *flagString, u8 (*getFlagFun)(void))
// {
//     serial_send_string(USART3, flagString);
//     serialTimeFlag = 1;
//     serialTime     = 0;
//     while (!getFlagFun()) {
//         serial_process_packet();
//         if (serialTimeFlag && serialTime > SERIAL_TIMEOUT) {
//             serialTime = 0;
//             serial_send_string(USART3, flagString);
//         }
//     }
//     serialTimeFlag = 0;
//     serialTime     = 0; // 清除计时器
// }
// 
// /******************************************************************
//  * \brief  if 型串口等待机
//  * \param  flagString* 要发送的标志字符串
//  * \param  getFlagFun* 获取标志位的函数指针
//  * \retval 0 获取失败，1 获取成功
//  *
//  * \note 该函数用于非阻塞等待，适用于需要在主循环中处理数据包的场景
//  */
// u8 serial_wait_if(u8 *flagString, u8 (*getFlagFun)(void))
// {
//     if (!serialTimeFlag) // 如果 serialTimeFlag 为 0，表示首次调用
//     {
//         serial_send_string(USART3, flagString);
//         serialTimeFlag = 1;
//         return 0; // 由于串口解析是在主循环中进行的，这里不阻塞等待
//     }
// 
//     if (!getFlagFun()) {
//         // serial_process_packet(); // 非阻塞等待，主循环会处理数据包
//         if (serialTimeFlag && serialTime > SERIAL_TIMEOUT) {
//             serialTime = 1; // 从 1 开始计时，防止 serialTime 为 0 时直接发送
//             serial_send_string(USART3, flagString);
//         }
//         return 0; // 获取失败
//     }
// 
//     serialTimeFlag = 0;
//     serialTime     = 0;
//     return 1; // 获取成功
// }

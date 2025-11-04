#include "serial.h"

/* -------------------------------- Static Constants */

static const u32 srlRccUart[4] = {0, RCC_APB2Periph_USART1, RCC_APB1Periph_USART2, RCC_APB1Periph_USART3};
static const u32 srlRccGpio[4] = {0, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB};

static const GPIO_TypeDef *srlGpioPort[4]  = {0, GPIOA, GPIOA, GPIOB};
static const USART_TypeDef *srlUartPort[4] = {0, USART1, USART2, USART3};
static const enum IRQn srlUartIRQn[4]      = {0, USART1_IRQn, USART2_IRQn, USART3_IRQn};

static const u16 srlGpioPinTx[4] = {0, GPIO_Pin_9, GPIO_Pin_2, GPIO_Pin_10};
static const u16 srlGpioPinRx[4] = {0, GPIO_Pin_10, GPIO_Pin_3, GPIO_Pin_11};

/* -------------------------------- Global Variables */

// 串口接收缓冲区

volatile u8 srlSigBuf[SRL_BUF_SLEN];
volatile u8 srlSigFlg = 0;
volatile u8 srlPidBuf[SRL_BUF_MLEN];
volatile u8 srlPidFlg = 0;
volatile u8 srlCmdBuf[SRL_BUF_LLEN];
volatile u8 srlCmdFlg = 0;
volatile u8 srlPkgBuf[SRL_BUF_LLEN];
volatile u8 srlPkgFlg = 0; 

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
    RCC_APB2PeriphClockCmd(srlRccGpio[srlNum], ENABLE);     // 开启GPIOx的时钟（所有 GPIO 都在 APB2）

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

/******************************************************************
 * \brief  解析 srlSigBuf 数据包内容
 * \note   在主循环中调用，当 srlSigFlg 为 1 时
 */
void serial_decode_sig(void)
{
    if (!srlSigFlg)
        return;

    // pass

    srlSigFlg = 0;
    if (debugFlag)
        serial_printf(USART2, "> Signal\n");
}

/******************************************************************
 * \brief  解析 srlPkgBuf 数据包内容
 * \note   在主循环中调用，当 srlPkgFlg 为 1 时
 */
void serial_decode_pkg(void)
{
    if (!srlPkgFlg)
        return;

    srlPkgFlg = 0;
    if (debugFlag)
        serial_printf(USART2, "> Package\n");
}

/******************************************************************
 * \brief  解析 srlPidBuf 数据包内容
 * \note   在主循环中调用，当 srlPidFlg 为 1 时
 */
void serial_decode_pid(void)
{
    if (!srlPidFlg)
        return;

    u8 len = srlPidBuf[0];
    if (len < 4) { // 至少要有一个字节
        srlPidFlg = 0;
        return;
    }

    // 数据实际从 srlPidBuf[1] 开始
    char type = srlPidBuf[1];
    u8 num    = srlPidBuf[2] - '0';
    float v   = strtof((char *)&srlPidBuf[4], NULL); // 从第4个字节开始转换为浮点数

    // e.g. {Pn 0.05}

    // switch (type) {
    //     case 'P':
    //         pidValue[num].Kp = v;
    //         break;
    //     case 'I':
    //         pidValue[num].Ki = v;
    //         break;
    //     case 'D':
    //         pidValue[num].Kd = v;
    //         break;
    //     default:
    //         break;
    // }

    srlPidFlg = 0;
    if (debugFlag)
        serial_printf(USART2, "> PID\n");
}

/******************************************************************
 * \brief  解析 srlCmdBuf 数据包内容
 * \note   在主循环中调用，当 srlCmdFlg 为 1 时
 * \note   支持的命令：
 *           srl -r <x> <string>  向串口x发送字符串
 *           debug -i/-o          打开/关闭调试模式
 *           led -i/-o <x>        打开/关闭 LEDx
 * \note   Q1: 为什么要用传递指针 rCmd？
 *         A1: 若直接将 strmatch_s 的值赋给 cCmd，一旦第一个条件不满足，其值会直接变成 NULL，导致后续判断无法进行
 */
void serial_decode_cmd(void)
{
    if (!srlCmdFlg)
        return;

    u8 *cCmd = srlCmdBuf + 1; // 正文指针
    u8 *rCmd = NULL;          // 传递指针
    u8 arg = 0;

    if (rCmd = strmatch_s(cCmd, "srl")) {
        if (cCmd = strmatch_s(rCmd, "-r")) {
            arg = (u8)strtof(cCmd, &cCmd);
            serial_printf(srlUartPort[arg], "%s\r\n", cCmd + 1);
        }
    } else if (rCmd = strmatch_s(cCmd, "debug")) {
        if (strmatch_s(rCmd, "-i"))
            debugFlag = 1;
        else if (strmatch_s(rCmd, "-o"))
            debugFlag = 0;
    } else if (rCmd = strmatch_s(cCmd, "led")) {
        if (cCmd = strmatch_s(rCmd, "-i")) {
            arg = strtof(cCmd, NULL);
            led_on(arg);
        } else if (cCmd = strmatch_s(rCmd, "-o")) {
            arg = strtof(cCmd, NULL);
            led_off(arg);
        }
    } else if (infoFlag) {
        serial_printf(USART2, "> Unknown CMD\n");
    }

    srlCmdFlg = 0;
    if (debugFlag)
        serial_printf(USART2, "> CMD: %s\n", srlCmdBuf + 1);
}

/*







*/

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
//         if (serialTimeFlag && serialTime > SRL_TIMEOUT) {
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
//         if (serialTimeFlag && serialTime > SRL_TIMEOUT) {
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

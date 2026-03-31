#include "serial.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const srl_s srlList[] = {
    [usart1] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_USART1, GPIOA, USART1, USART1_IRQn, GPIO_Pin_9, GPIO_Pin_10},
    [usart2] = {RCC_APB2Periph_GPIOA, RCC_APB1Periph_USART2, GPIOA, USART2, USART2_IRQn, GPIO_Pin_2, GPIO_Pin_3},
    [usart3] = {RCC_APB2Periph_GPIOB, RCC_APB1Periph_USART3, GPIOB, USART3, USART3_IRQn, GPIO_Pin_10, GPIO_Pin_11},
};

/* Global Variables -------------------------------------------------------- */

u8 srlReFlag = 0; // 串口调试返回标志位

// 串口接收缓存

__IO u8 srlSigBuf[SRL_BUF_SLEN];
__IO u8 srlSigFlg = 0;
__IO u8 srlPidBuf[SRL_BUF_MLEN];
__IO u8 srlPidFlg = 0;
__IO u8 srlCmdBuf[SRL_BUF_LLEN];
__IO u8 srlCmdFlg = 0;
__IO u8 srlPkgBuf[SRL_BUF_LLEN];
__IO u8 srlPkgFlg = 0;

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      串口初始化
 * \param[in]  idx 串口索引
 * \param[in]  baudRate 波特率
 * \param[in]  priority 优先级，格式：抢占优先级+响应优先级. 例：01 表示抢占优先级0，响应优先级1
 * \note       请确保私有量 srlList 已正确配置
 */
void serial_init(srl_e idx, u32 baudRate, u8 priority)
{
    /* 开启时钟 */
    if (idx == usart1)
        RCC_APB2PeriphClockCmd(srlList[idx].rccUart, ENABLE); // USART1 在 APB2
    else
        RCC_APB1PeriphClockCmd(srlList[idx].rccUart, ENABLE); // USART2/3 在 APB1
    RCC_APB2PeriphClockCmd(srlList[idx].rccGpio, ENABLE);     // 开启GPIOx的时钟（所有 GPIO 都在 APB2）

    /* GPIO 初始化 */
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin         = srlList[idx].tx;  // Tx 引脚
    GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz; // GPIO速度
    GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_Init(srlList[idx].gpio, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = srlList[idx].rx;       // Rx 引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(srlList[idx].gpio, &GPIO_InitStructure);

    /* USART 初始化 */
    USART_InitTypeDef USART_InitStructure         = {0};
    USART_InitStructure.USART_BaudRate            = baudRate;                       // 波特率
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;            // 8位数据位
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;               // 1位停止位
    USART_InitStructure.USART_Parity              = USART_Parity_No;                // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;  // 收发模式
    USART_Init(srlList[idx].uart, &USART_InitStructure);                            // 将结构体变量交给USART_Init，配置USARTx

    /* 中断输出配置 */
    USART_ITConfig(srlList[idx].uart, USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /* NVIC中断分组 */
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 配置NVIC为分组1 (抢占0~1，响应0~7)

    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure                  = {0};
    NVIC_InitStructure.NVIC_IRQChannel                   = srlList[idx].irqn;            // 选择配置NVIC的USART3线
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptingPriority(priority); // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = SubPriority(priority);        // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;                       // 指定NVIC线路使能
    NVIC_Init(&NVIC_InitStructure);                                                      // 将结构体变量交给NVIC_Init，配置NVIC外设

    /* USART使能 */
    USART_Cmd(srlList[idx].uart, ENABLE); // 使能USARTx，串口开始运行
}

/* ******************** 初始化函数 */

/*





*/

/* 发送函数 ******************** */

/******************************************************************
 * \brief      串口发送单字节
 * \param[in]  idx 串口索引
 * \param[in]  byte 要发送的字节
 */
void serial_send_byte(srl_e idx, u8 byte)
{
    USART_SendData(srlList[idx].uart, byte);
    while (USART_GetFlagStatus(srlList[idx].uart, USART_FLAG_TXE) == RESET);
}

/******************************************************************
 * \brief      串口发送字符串
 * \param[in]  idx 串口索引
 * \param[in]  str 要发送的字符串
 */
void serial_send_string(srl_e idx, u8 *str)
{
    u8 i;
    for (i = 0; str[i] != '\0'; i++)
        serial_send_byte(idx, str[i]);
}

/******************************************************************
 * \brief      串口格式化发送函数
 * \param[in]  idx 串口索引
 * \param[in]  format 格式化字符串
 * \param[in]  ...    可变参数列表
 */
void serial_printf(srl_e idx, u8 *format, ...)
{
    u8 str[SRL_SEND_LEN] = {0};
    va_list arg;
    va_start(arg, format);
    vsprintf((char *)str, (const char *)format, arg);
    va_end(arg);
    serial_send_string(idx, str);
}

/* ******************** 发送函数 */

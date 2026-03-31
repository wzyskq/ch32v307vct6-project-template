/******************************************************************
 ** \file    zdt_api.c
 **
 ** \author  Yiiry
 **
 ** \brief  本文件（zdt_api.c/.h）主要功能为处理电机返回数据包的接收、解析
 **
 ** \pre    需配合 Emm_V5.c/.h 使用
 **
 ** \note   张大头闭环步进电机本身就是集成好的闭环系统，一般情况下直接用 Emm_V5.c/.h 提供的命令控制电机即可.
 **         但如果需要对电机返回的数据包进行分析处理就比较麻烦，所以笔者就封装了这一套 API 来处理电机返回数据包的接收和解析.
 **         主要包括如下三个函数：
 **
 **         - zdt_irqHandler() 串口中断中调用，缓存命令.
 **
 **         - zdt_irqEndHandler() 定时中断中调用，终止缓存命令，最大超时时长由 **定时中断时基** 和 **超时判断周期** 两者乘积决定.
 **
 **         - Emm_V5_Get_Sys_Params() 定时中断中调用，解析缓存并传入的 zdtSysData 指针.
 **
 **         发送数据前已做阻塞判断处理，但建议在每次调用 Emm_V5 命令前先判断 zdtTxFlg 状态来避免数据冲突.
 **         判断状态方式可采用短时阻塞（逻辑清晰但浪费部分处理资源）或跳出逻辑（系统调度及时但复杂化上下逻辑）两种方式.
 **         具体示例如下.
 **
 ** \example  // 判断状态方式一：短时阻塞
 **             while (zdtTvFlg); // 等待清零
 **             Emm_V5_Vel_Control(4, 1, 0, 60, 0, false);
 **             while (zdtTvFlg); // 等待清零
 **             Emm_V5_Vel_Control(4, 1, 0, 30, 0, false);
 **
 ** \example  // 判断状态方式二：跳出逻辑
 **             if (!zdtTvFlg) {
 **                 Emm_V5_Vel_Control(4, 1, 0, 60, 0, false);
 **                 // ... 其他命令 ...
 **             } else return; // 或者其他处理方式
 **
 ** \example  // 获取实时数据
 **             Emm_V5_Read_Sys_Params(4, 1, S_VEL);                   // 申请获取速度
 **             while (zdtTvFlg);                                      // 等待更新数据
 **             serial_printf(1, "v = %d rpm\n", (s32)zdtSysData.vel); // 串口打印数据
 **
 ** \note   Tips:
 **         - 减小定时器时基和最大超时周期可以提高数据接收的实时性，但过小也可能会增加丢包风险.
 */

#include "zdt_api.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

// 存储

zdtSysData_t zdtSysData;

// 调试 & 安全

__IO bool zdtReFlag     = 0; // 串口调试返回标志位
__IO bool zdtTimeoutFlg = 0; // 串口接收超时计时标志位
__IO u32 zdtTimeoutCnt  = 0; // 串口接收超时计数

// 接收 & 处理

__IO u8 zdtRxIdx                = 0;   // 串口接收数据状态机
__IO u8 zdtTimCnt               = 0;   // 定时器周期计数变量
__IO bool zdtTimFlg             = 0;   // 定时器周期标志位
__IO bool zdtRvFlg              = 0;   // 串口返回数据标志位
__IO bool zdtTvFlg              = 0;   // 串口发送数据标志位
__IO u8 zdtRvBuf[ZDT_RVBUF_LEN] = {0}; // 串口返回数据包（有效值从 1 开始）
u8 zdtTvTag[ZDT_NUM]            = {0}; // 串口返回数据信息 [地址, 功能码]

/* Global Functions -------------------------------------------------------- */

/* 工具函数 ******************** */

/******************************************************************
 * \brief      串口发送十六进制字符串
 * \param[in]  idx 串口索引
 * \param[in]  data 要发送的十六进制数据字符串，必须以 '\0' 结尾
 */
void zdt_srl_send_hexString(srl_e idx, __IO u8 *data)
{
    u16 i = 0;
    for (;; i++) {
        serial_printf(idx, "%02X ", data[i]);
        if (data[i] == 0x6B && data[i + 1] == '\0')
            break;
    }
}

/******************************************************************
 * \brief      串口接收超时处理
 * \note       在 TIMx 中断服务函数中调用，以防止系统阻塞
 * \example    // 假设用于 TIM6：
 *              void TIM6_IRQHandler(void)
 *              {
 *                  if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET) {
 *                      // ... 其他代码 ...
 *                      zdt_irqEndHandler();
 *                      // ... 其他代码 ...
 *                      TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
 *                  }
 *              }
 */
void zdt_irqTimeoutHandler(void)
{
    if (zdtTimeoutFlg && zdtTvFlg)
        zdtTimeoutCnt++;

    if (zdtTimeoutCnt >= (ZDT_TIMEOUT * 2)) { // 接收超时时长
        zdtTimeoutFlg = false;                // 设置超时标志位
        zdtTvFlg      = false;                // 取消发送标志位，允许发送新数据
        zdtTimeoutCnt = 0;                    // 重置超时计数

        // 丢弃历史会话
        zdtRxIdx = 0;     // 重置接收状态
        zdtRvFlg = false; // 取消数据标志位

        if (zdtReFlag)
            serial_send_string(ZDT_RESRL, "ZDT:  WARNING: Receiving timeout!\n");
    }
}

/* ******************** 工具函数 */

/*





*/

/* 接收函数 ******************** */

/******************************************************************
 * \brief      电机中断请求处理
 * \param[in]  usart USARTx
 * \note       在 USARTx 中断服务函数中调用
 * \example    // 假设用于 USART1：
 *              void USART1_IRQHandler(void)
 *              {
 *                  zdt_irqHandler(USART1);
 *              }
 * \warning    严禁在此函数内调用任何阻塞式函数，否则会导致丢包等一系列问题！！！
 */
void zdt_irqHandler(USART_TypeDef *usart)
{
    if (USART_GetITStatus(usart, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(usart);
        if (!zdtRxIdx) {
            zdtRxIdx    = 1;
            zdtRvBuf[0] = 0; // 长度位清零
        }

        if (zdtRxIdx == 1) {
            if (zdtRvBuf[0] < ZDT_RVBUF_LEN - 1) { // 确保不会越界
                zdtRvBuf[++zdtRvBuf[0]] = rxData;

                if (rxData == 0x6B) { // 0x6B 触发结束计时
                    zdtTimCnt = 0;
                    zdtTimFlg = true; // 开始计时
                } else {
                    zdtTimFlg = false; // 取消计时
                }
            } else {
                zdtRxIdx = 0; // 丢弃过长数据包
            }
        }

        USART_ClearITPendingBit(usart, USART_IT_RXNE);
    }
}

/******************************************************************
 * \brief      电机终止中断请求处理
 * \param[in]  srlIdx 串口索引
 * \note       在 TIMx 中断服务函数中调用
 * \example    // 假设用于 TIM6：
 *              void TIM6_IRQHandler(void)
 *              {
 *                  if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET) {
 *                      // ... 其他代码 ...
 *                      zdt_irqEndHandler();
 *                      // ... 其他代码 ...
 *                      TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
 *                  }
 *              }
 */
void zdt_irqEndHandler(void)
{
    if (zdtTimFlg) {
        zdtTimCnt++;
        if (zdtTimCnt >= ZDT_IRQEND_PERIOD) { // 结束接收判断最大超时周期
            zdtTimFlg               = false;
            zdtRvFlg                = true; // 设置数据标志位
            zdtRvBuf[++zdtRvBuf[0]] = '\0'; // 字符串结束符
            zdtRxIdx                = 0;    // 重置接收状态

            // 已判定接收完成，关闭超时计时
            zdtTimeoutFlg = false;
            zdtTimeoutCnt = 0;
        }
    }
}

/* ******************** 接收函数 */

/*





*/

/* 处理函数 ******************** */

/******************************************************************
 * \brief    解析电机返回数据包内容
 * \note     在 TIMx 中断服务函数中调用
 *           - 暂时仅写了部分解析代码，其他部分可参考示例自行补充.
 * \example  // 请放置在 zdt_irqEndHandler(); 之后以确保数据包已接收完成
 *              // ... 其他代码 ...
 *              zdt_irqEndHandler();
 *              Emm_V5_Get_Sys_Params(&zdtSysData);
 *              // ... 其他代码 ...
 */
void Emm_V5_Get_Sys_Params(zdtSysData_t *data)
{
    if (!zdtRvFlg || data == NULL)
        return;

    // serial_printf(1, "ZDT: Rv: %02X %02X %02X %02X\n", zdtRvBuf[1], zdtRvBuf[2], zdtRvBuf[3], zdtRvBuf[4]);

    if (zdtRvBuf[1] != zdtTvTag[0]) { // 地址不匹配，全部丢弃
        zdtRvFlg      = false;
        zdtTvFlg      = false;
        zdtTimeoutFlg = false;
        zdtTimeoutCnt = 0;
        return;
    }

    u32 v = 0;

    switch (zdtRvBuf[2]) {
        case 0x24: // S_VBUS 读取总线电压

            break;
        case 0x26: // S_CBUS 读取总线电流

            break;
        case 0x27: // S_CPHA 读取相电流

            break;
        case 0x29: // S_ENCO 读取编码器原始值

            break;
        case 0x30: // S_CLKC 读取实时脉冲数

            break;
        case 0x31: // S_ENCL 读取线性化后编码器值

            break;
        case 0x32: // S_CLKI 读取输入脉冲数

            break;
        case 0x33: { // S_TPOS 读取目标位置
            v          = (zdtRvBuf[4] << 24) | (zdtRvBuf[5] << 16) | (zdtRvBuf[6] << 8) | zdtRvBuf[7];
            float deg  = (float)v * 360.0f / 65536.0f; // 转换为度数
            data->tpos = (!zdtRvBuf[3]) ? deg : -deg;
            break;
        }
        case 0x34: // S_SPOS 读取实时设定目标位置

            break;
        case 0x35: { // S_VEL 读取实时转速
            v         = (zdtRvBuf[4] << 8) | zdtRvBuf[5];
            float vel = (float)v;
            data->vel = (!zdtRvBuf[3]) ? vel : -vel;
            break;
        }
        case 0x36: { // S_CPOS 读取实时位置
            v          = (zdtRvBuf[4] << 24) | (zdtRvBuf[5] << 16) | (zdtRvBuf[6] << 8) | zdtRvBuf[7];
            float deg  = (float)v * 360.0f / 65536.0f; // 转换为度数
            data->cpos = (!zdtRvBuf[3]) ? deg : -deg;
            break;
        }
        case 0x37: // S_PERR 读取位置误差

            break;
        case 0x38: // S_VBAT 读取多圈编码器电池电压（Y42）

            break;
        case 0x39: // S_TEMP 读取实时温度（Y42）

            break;
        case 0x3A: // S_FLAG 读取状态标志位

            break;
        case 0x3B: // S_OFLAG 读取回零状态标志位

            break;
        case 0x3C: // S_OAF 读取状态+回零标志位（Y42）

            break;
        case 0x3D: // S_PIN 读取引脚状态（Y42）

            break;
        default: // 非读取命令的返回数据包/未知标签
            if (zdtRvBuf[3] != 0x02)
                serial_printf(ZDT_RESRL, "ZDT:  Err: %02X %02X %02X\n", zdtRvBuf[1], zdtRvBuf[2], zdtRvBuf[3]);
            break;
    }

    if (zdtReFlag) {
        serial_send_string(ZDT_RESRL, "ZDT:  Re: ");
        zdt_srl_send_hexString(ZDT_RESRL, zdtRvBuf);
        serial_send_byte(ZDT_RESRL, '\n');
    }

    zdtRvFlg      = false;
    zdtTvFlg      = false; // 解析完成，允许下一次发送
    zdtTimeoutFlg = false; // 取消超时计时
    zdtTimeoutCnt = 0;
}

/* ******************** 处理函数 */

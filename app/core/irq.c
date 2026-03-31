#include "irq.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* TIM 中断请求 ******************** */

/******************************************************************
 * \brief  TIM6 中断请求处理函数
 * \note   步进电机解析时基（中断周期 0.5ms）
 */
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET) {

        // 超时计时
        zdt_irqTimeoutHandler();

        // 电机解析
        zdt_irqEndHandler();
        Emm_V5_Get_Sys_Params(&zdtSysData);

        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}

/******************************************************************
 * \brief  TIM7 中断请求处理函数
 * \note   系统任务调度时基（中断周期 10ms）
 */
void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) {

        // 时间刷新
        sysTime++;

        timer_pwmIn_readData(tim3, &timPwmInData); // 读取 PWM 输入测量结果

        // whlCnt = timer_encoder_readCnt(tim3, normal); // 读取编码器计数值

        u32 t = sysTime / 100;

        if (t % 2 == 0) {
            led_on(led2);
            servo_set_pos(tim10, 3, 0); 
        } else {
            led_off(led2);
            servo_set_pos(tim10, 3, 180);
        }

        // Emm_V5_Vel_Control(ZDT_SRL, 1, t % 2, 60, 0, false);
        // Emm_V5_Vel_Control(ZDT_SRL, 2, t % 2, 60, 0, false);

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}

/* ******************** TIM 中断请求 */

/*





*/

/* EXTI 中断请求 ******************** */

void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2) != RESET) {

        serial_printf(usart1, "EXTI2\n");

        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

/* ******************** EXTI 中断请求 */

/*





*/

/* USART 中断请求 ******************** */

/******************************************************************
 * @fn     USART1_IRQHandler
 * @brief  该函数处理 USART1 中断请求
 */
void USART1_IRQHandler(void)
{
    // if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
    //     u8 rxData = USART_ReceiveData(USART1);
    //     serial_printf(usart1, "%c", rxData);
    //     USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    // }

    __IO static u8 rxIdx = 0;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART1);
        if (rxData == '(') {
            rxIdx        = 2;
            srlCmdBuf[0] = 0; // 长度位清零
        } else if (rxData == '<') {
            rxIdx        = 3;
            srlPidBuf[0] = 0; // 长度位清零
        } else if (rxData == 0xEE) {
            rxIdx = 5;
        }

        else if (rxIdx == 2) {
            if (rxData == ')') {
                srlCmdBuf[srlCmdBuf[0] + 1] = '\0'; // 字符串结束符
                srlCmdFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlCmdBuf[0] < SRL_BUF_LLEN - 1) {
                srlCmdBuf[++srlCmdBuf[0]] = rxData;
            }
        } else if (rxIdx == 3) {
            if (rxData == '>') {
                srlPidBuf[srlPidBuf[0] + 1] = '\0'; // 字符串结束符
                srlPidFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlPidBuf[0] < SRL_BUF_MLEN - 1) {
                srlPidBuf[++srlPidBuf[0]] = rxData;
            }
        } else if (rxIdx == 5) {
            serial_send_byte(ZDT_SRL, rxData); // 转发命令
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART3_IRQHandler
 * @brief  该函数处理 USART3 中断请求
 */
void USART3_IRQHandler(void)
{
    zdt_irqHandler(USART3);
}

/******************************************************************
 * @fn     USART2_IRQHandler
 * @brief  该函数处理 USART2 中断请求
 */
void USART2_IRQHandler(void)
{
    __IO static u8 rxIdx = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART2);
        if (rxData == '(') {
            rxIdx        = 2;
            srlCmdBuf[0] = 0; // 长度位清零
        } else if (rxData == '<') {
            rxIdx        = 3;
            srlPidBuf[0] = 0; // 长度位清零
        }

        else if (rxIdx == 2) {
            if (rxData == ')') {
                srlCmdBuf[srlCmdBuf[0] + 1] = '\0'; // 字符串结束符
                srlCmdFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlCmdBuf[0] < SRL_BUF_LLEN - 1) {
                srlCmdBuf[++srlCmdBuf[0]] = rxData;
            }
        } else if (rxIdx == 3) {
            if (rxData == '>') {
                srlPidBuf[srlPidBuf[0] + 1] = '\0'; // 字符串结束符
                srlPidFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlPidBuf[0] < SRL_BUF_MLEN - 1) {
                srlPidBuf[++srlPidBuf[0]] = rxData;
            }
        }
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

// /******************************************************************
//  * @fn     USART3_IRQHandler
//  * @brief  该函数处理 USART3 中断请求
//  */
// void USART3_IRQHandler(void)
// {
//     volatile static u8 rxIdx = 0;
//     if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
//         u8 rxData = USART_ReceiveData(USART3);
//         if (rxData == '{') {
//             rxIdx        = 1;
//             srlSigBuf[0] = 0; // 长度位清零
//         } else if (rxData == '[') {
//             rxIdx        = 4;
//             srlPkgBuf[0] = 0; // 长度位清零
//         }
//
//         else if (rxIdx == 1) {
//             if (rxData == '}') {
//                 srlSigBuf[srlSigBuf[0] + 1] = '\0'; // 字符串结束符
//                 srlSigFlg                   = 1;    // 接收完成标志
//                 rxIdx                       = 0;
//             } else if (srlSigBuf[0] < SRL_BUF_SLEN - 1) {
//                 srlSigBuf[++srlSigBuf[0]] = rxData;
//             }
//         } else if (rxIdx == 4) {
//             if (rxData == ']') {
//                 srlPkgBuf[srlPkgBuf[0] + 1] = '\0'; // 字符串结束符
//                 srlPkgFlg                   = 1;    // 接收完成标志
//                 rxIdx                       = 0;
//             } else if (srlPkgBuf[0] < SRL_BUF_LLEN - 1) {
//                 srlPkgBuf[++srlPkgBuf[0]] = rxData;
//             }
//         }
//
//         USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//     }
// }

// volatile static u8 rxIdx = 0;
// if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET) {
//     u8 rxData = USART_ReceiveData(USARTx);
//     if (rxData == '{') {
//         rxIdx        = 1;
//         srlSigBuf[0] = 0; // 长度位清零
//     } else if (rxData == '(') {
//         rxIdx        = 2;
//         srlCmdBuf[0] = 0; // 长度位清零
//     } else if (rxData == '<') {
//         rxIdx        = 3;
//         srlPidBuf[0] = 0; // 长度位清零
//     } else if (rxData == '[') {
//         rxIdx        = 4;
//         srlPkgBuf[0] = 0; // 长度位清零
//     }
//
//     else if (rxIdx == 1) {
//         if (rxData == '}') {
//             srlSigFlg = 1; // 接收完成标志
//             rxIdx     = 0;
//         } else if (srlSigBuf[0] < SRL_BUF_SLEN - 1) {
//             srlSigBuf[++srlSigBuf[0]] = rxData;
//         }
//     } else if (rxIdx == 2) {
//         if (rxData == ')') {
//             srlCmdFlg = 1; // 接收完成标志
//             rxIdx     = 0;
//         } else if (srlCmdBuf[0] < SRL_BUF_MLEN - 1) {
//             srlCmdBuf[++srlCmdBuf[0]] = rxData;
//         }
//     } else if (rxIdx == 3) {
//         if (rxData == '>') {
//             srlPidFlg = 1; // 接收完成标志
//             rxIdx     = 0;
//         } else if (srlPidBuf[0] < SRL_BUF_MLEN - 1) {
//             srlPidBuf[++srlPidBuf[0]] = rxData;
//         }
//     } else if (rxIdx == 4) {
//         if (rxData == ']') {
//             srlPkgFlg = 1; // 接收完成标志
//             rxIdx     = 0;
//         } else if (srlPkgBuf[0] < SRL_BUF_LLEN - 1) {
//             srlPkgBuf[++srlPkgBuf[0]] = rxData;
//         }
//     }
// }

/* ******************** USART 中断请求 */

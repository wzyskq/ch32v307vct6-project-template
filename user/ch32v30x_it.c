/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/03/06
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32v30x_it.h"

/******************************************************************
 * @fn     NMI_Handler
 * @brief  该函数处理 NMI 中断请求
 */
void NMI_Handler(void)
{
    while (1) {
    }
}

/******************************************************************
 * @fn     HardFault_Handler
 * @brief  该函数处理 Hard Fault 中断请求
 */
void HardFault_Handler(void)
{
    NVIC_SystemReset();
    while (1) {
    }
}

/******************************************************************
 * @fn     USART1_IRQHandler
 * @brief  该函数处理 USART1 中断请求
 */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART1);
        serial_printf(USART1, "%c", rxData);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART2_IRQHandler
 * @brief  该函数处理 USART2 中断请求
 */
void USART2_IRQHandler(void)
{
    volatile static u8 rxNum = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART2);
        if (rxData == '(') {
            rxNum        = 2;
            srlCmdBuf[0] = 0; // 长度位清零
        } else if (rxData == '<') {
            rxNum        = 3;
            srlPidBuf[0] = 0; // 长度位清零
        }

        else if (rxNum == 2) {
            if (rxData == ')') {
                srlCmdBuf[srlCmdBuf[0] + 1] = '\0'; // 字符串结束符
                srlCmdFlg                   = 1;    // 接收完成标志
                rxNum                       = 0;
            } else if (srlCmdBuf[0] < SRL_BUF_LLEN - 1) {
                srlCmdBuf[++srlCmdBuf[0]] = rxData;
            }
        } else if (rxNum == 3) {
            if (rxData == '>') {
                srlPidBuf[srlPidBuf[0] + 1] = '\0'; // 字符串结束符
                srlPidFlg                   = 1;    // 接收完成标志
                rxNum                       = 0;
            } else if (srlPidBuf[0] < SRL_BUF_MLEN - 1) {
                srlPidBuf[++srlPidBuf[0]] = rxData;
            }
        }
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART3_IRQHandler
 * @brief  该函数处理 USART3 中断请求
 */
void USART3_IRQHandler(void)
{
    volatile static u8 rxNum = 0;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART3);
        if (rxData == '{') {
            rxNum        = 1;
            srlSigBuf[0] = 0; // 长度位清零
        } else if (rxData == '[') {
            rxNum        = 4;
            srlPkgBuf[0] = 0; // 长度位清零
        }

        else if (rxNum == 1) {
            if (rxData == '}') {
                srlSigBuf[srlSigBuf[0] + 1] = '\0'; // 字符串结束符
                srlSigFlg                   = 1;    // 接收完成标志
                rxNum                       = 0;
            } else if (srlSigBuf[0] < SRL_BUF_SLEN - 1) {
                srlSigBuf[++srlSigBuf[0]] = rxData;
            }
        } else if (rxNum == 4) {
            if (rxData == ']') {
                srlPkgBuf[srlPkgBuf[0] + 1] = '\0'; // 字符串结束符
                srlPkgFlg                   = 1;    // 接收完成标志
                rxNum                       = 0;
            } else if (srlPkgBuf[0] < SRL_BUF_LLEN - 1) {
                srlPkgBuf[++srlPkgBuf[0]] = rxData;
            }
        }

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

// volatile static u8 rxNum = 0;
// if (USART_GetITStatus(USARTx, USART_IT_RXNE) == SET) {
//     u8 rxData = USART_ReceiveData(USARTx);
//     if (rxData == '{') {
//         rxNum        = 1;
//         srlSigBuf[0] = 0; // 长度位清零
//     } else if (rxData == '(') {
//         rxNum        = 2;
//         srlCmdBuf[0] = 0; // 长度位清零
//     } else if (rxData == '<') {
//         rxNum        = 3;
//         srlPidBuf[0] = 0; // 长度位清零
//     } else if (rxData == '[') {
//         rxNum        = 4;
//         srlPkgBuf[0] = 0; // 长度位清零
//     }
//
//     else if (rxNum == 1) {
//         if (rxData == '}') {
//             srlSigFlg = 1; // 接收完成标志
//             rxNum     = 0;
//         } else if (srlSigBuf[0] < SRL_BUF_SLEN - 1) {
//             srlSigBuf[++srlSigBuf[0]] = rxData;
//         }
//     } else if (rxNum == 2) {
//         if (rxData == ')') {
//             srlCmdFlg = 1; // 接收完成标志
//             rxNum     = 0;
//         } else if (srlCmdBuf[0] < SRL_BUF_MLEN - 1) {
//             srlCmdBuf[++srlCmdBuf[0]] = rxData;
//         }
//     } else if (rxNum == 3) {
//         if (rxData == '>') {
//             srlPidFlg = 1; // 接收完成标志
//             rxNum     = 0;
//         } else if (srlPidBuf[0] < SRL_BUF_MLEN - 1) {
//             srlPidBuf[++srlPidBuf[0]] = rxData;
//         }
//     } else if (rxNum == 4) {
//         if (rxData == ']') {
//             srlPkgFlg = 1; // 接收完成标志
//             rxNum     = 0;
//         } else if (srlPkgBuf[0] < SRL_BUF_LLEN - 1) {
//             srlPkgBuf[++srlPkgBuf[0]] = rxData;
//         }
//     }
// }

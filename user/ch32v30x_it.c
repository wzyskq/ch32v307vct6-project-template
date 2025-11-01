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
        u8 res = USART_ReceiveData(USART1);
        serial_printf(USART1, "%c", res);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART2_IRQHandler
 * @brief  该函数处理 USART2 中断请求
 */
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
        u8 res = USART_ReceiveData(USART2);
        serial_printf(USART2, "%c", res);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART3_IRQHandler
 * @brief  该函数处理 USART3 中断请求
 */
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
        u8 res = USART_ReceiveData(USART3);
        serial_printf(USART3, "%c", res);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

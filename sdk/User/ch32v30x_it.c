// /********************************** (C) COPYRIGHT *******************************
//  * File Name          : ch32v30x_it.c
//  * Author             : WCH
//  * Version            : V1.0.0
//  * Date               : 2024/03/06
//  * Description        : Main Interrupt Service Routines.
//  *********************************************************************************
//  * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
//  * Attention: This software (modified or not) and binary are used for
//  * microcontroller manufactured by Nanjing Qinheng Microelectronics.
//  *******************************************************************************/
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

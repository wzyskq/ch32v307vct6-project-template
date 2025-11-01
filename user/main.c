/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/

#include "main.h"

/* Global typedef */

/* Global define */

/* Global Variable */

/*********************************************************************
 * \fn      main
 * \brief   主程序
 * \note    本项目基于 CH32V307VCT6 评估板进行开发
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    delay_init();

    leds_init();

    while (1)
    {
        led_on(1);
        led_off(2);
        delay_ms(500);
        led_off(1);
        led_on(2);
        delay_ms(500);
    }
}

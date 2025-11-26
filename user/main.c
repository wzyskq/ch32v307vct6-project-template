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

#include "main.h"

/* -------------------------------- Global Variable */

/* -------------------------------- Main Function */

/******************************************************************
 * \fn      main
 * \brief   主程序
 * \note    本项目基于 CH32V307VCT6 评估板进行开发
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    delay_init();

    // serial_init(1, 115200, 0); // USART1 初始化
    serial_init(2, 115200, 1); // USART2 初始化
    // serial_init(3, 115200, 2); // USART3 初始化

    leds_init();
    oled_init();
    // timer_pwmOut_init(3, 34, 14400, 200); // TIM3_CH3/CH4 PWM 输出初始化，频率 50Hz

    serial_printf(USART2, "USART2: online.\r\n");
    // oled_printf(0, 0, OLED_8X16, "System Init!");
    // oled_update();

    system_loop();
}

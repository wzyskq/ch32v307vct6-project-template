#ifndef __IRQ_H
#define __IRQ_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// TIM 中断请求

void TIM7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

// USART 中断请求

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

#endif

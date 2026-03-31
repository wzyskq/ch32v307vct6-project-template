#ifndef __IRQ_H
#define __IRQ_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Global Types ------------------------------------------------------------ */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// TIM 中断请求

void TIM6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

// EXTI 中断请求

void EXTI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

// USART 中断请求

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

#endif

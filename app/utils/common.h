#ifndef __COMMON_H
#define __COMMON_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// 阻塞延时

void delay_init(void);
void delay_us(u32 us);
void delay_ms(u32 ms);
void delay_s(u32 s);

#endif

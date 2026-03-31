#ifndef __COMMON_H
#define __COMMON_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Global Types ------------------------------------------------------------ */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// 阻塞延时

void delay_init(void);
void delay_us(u32 us);
void delay_ms(u32 ms);
void delay_s(u32 s);

// 字符串处理

bool strmatch(u8 *pattern, u8 *srcPtr, u8 **endPtr);
bool strmatch_s(u8 *pattern, u8 *srcPtr, u8 **endPtr);
void strtohex(u8 *str, u8 *hex);

#endif

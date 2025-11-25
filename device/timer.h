#ifndef __TIMER_H
#define __TIMER_H

#include "main.h"

/* -------------------------------- Global Functions */

/* 初始化函数 */

void timer_pwmOut_init(u8 timNum, u16 chNum, u16 arr, u16 psc);

/* 设置函数 */

void timer_pwmOut_setDuty(u8 timNum, u8 chNum, u16 duty);

#endif

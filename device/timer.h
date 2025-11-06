#ifndef __TIMER_H
#define __TIMER_H

#include "main.h"

/* -------------------------------- Global Functions */

void timer_pwmOut_init(u8 timNum, u8 chNum, u16 arr, u16 psc);
void timer_pwmOut_setDuty(u8 timNum, u8 chNum, u16 duty);

#endif

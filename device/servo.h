#ifndef __SERVO_H
#define __SERVO_H

#include "main.h"

/*
    使用下面的函数前，请先调用 timer_pwmOut_init()
    初始化对应的定时器、通道和 PWM 参数（ARR、PSC）
*/

/* -------------------------------- Global Functions */

void servo_set_spd(u8 timNum, u8 chNum, s16 spd);
void servo_set_pos(u8 timNum, u8 chNum, u8 pos);

#endif

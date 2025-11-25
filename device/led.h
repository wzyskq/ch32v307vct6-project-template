#ifndef __LED_H
#define __LED_H

#include "main.h"

/* -------------------------------- Exported Functions */

/* 初始化函数 */

void led_init(u8 ledNum);
void leds_init(void);

/* 设置函数 */

void led_on(u8 ledNum);
void led_off(u8 ledNum);
void led_toggle(u8 ledNum);
void leds_toggle(u8 ledBeginNum, u8 ledEndNum);

#endif
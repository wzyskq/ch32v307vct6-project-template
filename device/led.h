#ifndef __LED_H
#define __LED_H

#include "main.h"

/* -------------------------------- Exported Functions */

void led_init(u8 ledNum);
void leds_init(void);

void led_on(u8 ledNum);
void led_off(u8 ledNum);
void led_toggle(u8 ledNum);
void leds_toggle(u8 ledBeginNum, u8 ledEndNum);

#endif
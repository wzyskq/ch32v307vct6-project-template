#ifndef __LED_H
#define __LED_H

#include "main.h"

void led_init(u8 ledNum);
void leds_init(void);

void led_on(u8 ledNum);
void led_off(u8 ledNum);

#endif
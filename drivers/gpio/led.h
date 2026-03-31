#ifndef __LED_H
#define __LED_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

// LED 配置索引
typedef enum {
    led1 = 0,
    led2,
    ledNum // LED 末尾索引，仅用于统计 LED 数量
} led_e;

// LED 配置结构体
typedef struct {
    u32 rccGpio;
    GPIO_TypeDef *gpio;
    u16 pin;
    level_e ledOnLevel; // LED 点亮电平
} led_s;

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void led_init(led_e idx);
void leds_init(void);

// 设置函数

void led_on(u8 idx);
void led_off(u8 idx);
void led_turn(u8 idx);
void leds_turn(u8 begIdx, u8 endIdx);

#endif
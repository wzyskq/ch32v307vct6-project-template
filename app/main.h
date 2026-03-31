#ifndef __MAIN_H
#define __MAIN_H

/* System Includes --------------------------------------------------------- */

#include "ch32v30x.h"
#include "types.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

/* Global Macros ----------------------------------------------------------- */

// 常用工具宏

#define ABS(x)    ((x) > 0 ? (x) : -(x))  // 伪泛型绝对值
#define MIN(a, b) ((a) < (b) ? (a) : (b)) // 伪泛型最小值
#define MAX(a, b) ((a) > (b) ? (a) : (b)) // 伪泛型最大值

// 优先级重映射

#define PreemptingPriority(x) ((x) < 10 ? 0 : (x) / 10)
#define SubPriority(x) ((x) % 10)

// 接口重映射

#define RCC_GPIO(port)   ((u32)((u32)0x02U << (port)))
#define GPIO(port)       ((GPIO_TypeDef *)((u32)(APB2PERIPH_BASE + ((u32)port << 10) + 0x0400)))
#define PIN(pin)         ((u16)((u16)0x01U << (pin)))
#define PINS(start, end) ((0xFFFFUL << (start)) & (0xFFFFUL >> (15 - (u16)(end))))

/* Private Types ----------------------------------------------------------- */

/* User Includes ----------------------------------------------------------- */

// 业务及工具

#include "irq.h"
#include "sys.h"
#include "task.h"
#include "common.h"
#include "misc.h"

// 常用外设

#include "exter.h"
#include "key.h"
#include "led.h"
#include "oled.h"
#include "timer.h"
#include "servo.h"
#include "serial.h"

// 三方组件

#include "zdt_api.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// // 接口重映射
//
// extern u32 rcc_gpio_port(port_e port);
// extern GPIO_TypeDef *gpio_port(port_e port);
// extern u16 gpio_pin(u8 pin);

#endif

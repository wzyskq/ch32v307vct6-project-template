#ifndef __EXTER_H
#define __EXTER_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

// EXTI 配置索引
typedef enum {
    exti2 = 0,
} ext_e;

// EXTI 配置结构体
typedef struct {
    u32 rccGpio;
    GPIO_TypeDef *gpio;
    u16 pin;
    u8 portSrc;
    u8 pinSrc;
    u32 extiLine;
    EXTITrigger_TypeDef trigger;
    enum IRQn irqn;
} ext_s;

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void exter_init(ext_e idx, u8 priority);

#endif

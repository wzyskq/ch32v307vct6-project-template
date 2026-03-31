#ifndef __TYPES_H
#define __TYPES_H

/* Global Types ------------------------------------------------------------ */

// 端口类型
typedef enum {
    A = 1,
    B,
    C,
    D,
    E
} port_e;

// 电平类型
typedef enum {
    low  = 0,
    high = !low,
} level_e;

// 方向类型
typedef enum {
    normal = 0,
    inverse = !normal,
} sign_e;

/* timer.h ******************** */

// TIM 配置索引
typedef enum {
    tim1 = 0,
    tim2,
    tim3,
    tim4,
    tim5,
    tim6,
    tim7,
    tim8,
    tim9,
    tim10,
} tim_e;

/* serial.h ******************** */

// USART 配置索引
typedef enum {
    usart1 = 0,
    usart2,
    usart3,
} srl_e;

#endif

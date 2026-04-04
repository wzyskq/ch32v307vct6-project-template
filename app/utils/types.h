#ifndef __TYPES_H
#define __TYPES_H

/* Private Types ----------------------------------------------------------- */

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

#endif

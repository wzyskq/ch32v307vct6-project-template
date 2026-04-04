#ifndef __KEY_H_
#define __KEY_H_

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

// KEY 配置索引
typedef enum {
    key1 = 0,
    keyNum // KEY 末尾索引，仅用于统计 KEY 数量
} key_e;

// KEY 配置结构体
typedef struct {
    u32 rccGpio;
    GPIO_TypeDef *gpio;
    u16 pin;
    level_e keyOnLevel; // KEY 按下电平
} key_s;

/* Global Variables -------------------------------------------------------- */

extern u16 keyBox[];
extern u8 taskNum;

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void key_init(key_e idx);
void keys_init(void);

// 功能函数

bool key_read(key_e idx);
key_e key_scan(void);

#endif

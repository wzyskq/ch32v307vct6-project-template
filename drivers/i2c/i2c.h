/******************************************************************
 * \file    i2c.h
 * 
 * \brief  软件模拟 I2C 驱动代码说明
 * 
 * \note   - 本文件为软件模拟的 I2C 通信协议的通用驱动代码.
 *           参考教程：（江协版）https://blog.csdn.net/xiaobaivera/article/details/140551276.
 *
 *         - 为了增加代码的复用性，本文件不作初始化 GPIO 的工作，
 *           请在使用相应外设前调用 i2c_gpio_init(i2cIdx); 进行初始化.
 *           同时添加枚举类型 i2cObj_t 来区分不同的 I2C 设备.
 */

#ifndef __I2C_H_
#define __I2C_H_

#include "main.h"

/* Private Types ----------------------------------------------------------- */

// I2C 配置索引
typedef enum {
    i2c_OLED = 0,
    i2c_MPU6050,
} i2c_e;

// I2C 配置结构体
typedef struct {
    u32 rccGpio;
    GPIO_TypeDef *gpio;
    u16 scl;
    u16 sda;
} i2c_s;

/* Global Variables -------------------------------------------------------- */

extern __IO i2c_e i2cIdx;

/* Global Functions -------------------------------------------------------- */

// 通信工具

void i2c_w_SCL(u8 bit);
void i2c_w_SDA(u8 bit);

u8 i2c_r_SCL(void);
u8 i2c_r_SDA(void);

// 通信协议

void i2c_start(void);
void i2c_stop(void);
void i2c_pass(void);

void i2c_send_byte(u8 byte);
u8 i2c_recv_byte(void);

void i2c_send_ack(u8 ack);
u8 i2c_recv_ack(void);

// 初始化函数

void i2c_gpio_init(i2c_e idx);

#endif

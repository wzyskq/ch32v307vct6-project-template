/******************************************************************
 * \file    i2c.c
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

#include "i2c.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const i2c_s i2cList[] = {
    [i2c_OLED]    = {RCC_APB2Periph_GPIOD, GPIOD, GPIO_Pin_10, GPIO_Pin_11},
    // [MPU6050] = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_15, GPIO_Pin_14},
};

/* Global Variables -------------------------------------------------------- */

__IO i2c_e i2cIdx = i2c_OLED; // I2C 对象索引

/*





*/

/* Global Functions -------------------------------------------------------- */

/* 通信工具 ******************** */

/******************************************************************
 * \brief      I2C 延时函数
 * \param[in]  delay 延时参数，0..65535
 */
void i2c_delay(u16 delay)
{
    delay *= 20;
    while (delay--)
        __NOP(); // 空操作，延时
}

/******************************************************************
 * \brief      I2C 写入 SCL 引脚
 * \param[in]  bit 写入值，0/1
 */
void i2c_w_SCL(u8 bit)
{
    GPIO_WriteBit(i2cList[i2cIdx].gpio, i2cList[i2cIdx].scl, (BitAction)(bit));
    i2c_delay(1);
}

/******************************************************************
 * \brief      I2C 写入 SDA 引脚
 * \param[in]  bit 写入值，0/1
 */
void i2c_w_SDA(u8 bit)
{
    GPIO_WriteBit(i2cList[i2cIdx].gpio, i2cList[i2cIdx].sda, (BitAction)(bit));
    i2c_delay(1);
}

/******************************************************************
 * \brief      I2C 读取 SCL 引脚
 * \return     读取值，0/1
 */
u8 i2c_r_SCL(void)
{
    u8 bit = GPIO_ReadInputDataBit(i2cList[i2cIdx].gpio, i2cList[i2cIdx].scl);
    i2c_delay(1);
    return bit;
}

/******************************************************************
 * \brief      I2C 读取 SDA 引脚
 * \return     读取值，0/1
 */
u8 i2c_r_SDA(void)
{
    u8 bit = GPIO_ReadInputDataBit(i2cList[i2cIdx].gpio, i2cList[i2cIdx].sda);
    i2c_delay(1);
    return bit;
}

/* ******************** 通信工具 */

/*





*/

/* 通信协议 ******************** */

/******************************************************************
 * \brief      I2C 起始信号
 */
void i2c_start(void)
{
    i2c_w_SDA(1);
    i2c_w_SCL(1);
    i2c_w_SDA(0);
    i2c_w_SCL(0);
}

/******************************************************************
 * \brief      I2C 终止信号
 */
void i2c_stop(void)
{
    i2c_w_SDA(0);
    i2c_w_SCL(1);
    i2c_w_SDA(1);
}

/******************************************************************
 * \brief      I2C 时钟跳变
 */
void i2c_pass(void)
{
    i2c_w_SCL(1);
    i2c_w_SCL(0);
}

/******************************************************************
 * \brief      I2C 发送一个字节
 * \param[in]  byte 要发送的字节
 */
void i2c_send_byte(u8 byte)
{
    for (u8 i = 0; i < 8; i++) {
        i2c_w_SDA((byte & (0x80 >> i)));
        i2c_w_SCL(1);
        i2c_w_SCL(0);
    }
}

/******************************************************************
 * \brief      I2C 接收一个字节
 * \return     接收到的字节
 */
u8 i2c_recv_byte(void)
{
    u8 byte = 0x00;
    i2c_w_SDA(1); // 释放 SDA 总线
    for (u8 i = 0; i < 8; i++) {
        i2c_w_SCL(1);
        if (i2c_r_SDA()) byte |= (0x80 >> i);
        i2c_w_SCL(0);
    }
    return byte;
}

/******************************************************************
 * \brief      I2C 发送应答信号
 * \param[in]  ack 应答信号，0：ACK；1：NACK
 */
void i2c_send_ack(u8 ack)
{
    i2c_w_SDA(ack);
    i2c_w_SCL(1);
    i2c_w_SCL(0);
}

/******************************************************************
 * \brief      I2C 接收应答信号
 * \return     应答信号，0：ACK；1：NACK
 */
u8 i2c_recv_ack(void)
{
    u8 ack;
    i2c_w_SDA(1); // 释放 SDA 总线
    i2c_w_SCL(1);
    ack = i2c_r_SDA();
    i2c_w_SCL(0);
    return ack;
}

/* ******************** 通信协议 */

/*





*/

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      I2C GPIO 初始化
 * \param[in]  idx I2C 索引
 */
void i2c_gpio_init(i2c_e idx)
{
    /* 使能 GPIO 时钟 */
    RCC_APB2PeriphClockCmd(i2cList[idx].rccGpio, ENABLE);

    /* I2C 引脚配置 */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Pin   = i2cList[idx].scl | i2cList[idx].sda;
    GPIO_Init(i2cList[idx].gpio, &GPIO_InitStruct);

    /* 设置引脚初始状态 */
    i2cIdx = idx;
    i2c_w_SCL(1);
    i2c_w_SDA(1);
}

/* ******************** 初始化函数 */

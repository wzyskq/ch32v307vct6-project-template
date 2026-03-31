#ifndef __SERIAL_H
#define __SERIAL_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

#define SRL_RESRL    usart1 // 调试返回串口号
#define SRL_SEND_LEN 1024   // 串口发送缓冲区长度

#define SRL_BUF_SLEN 8   // 短缓冲区
#define SRL_BUF_MLEN 16  // 中缓冲区
#define SRL_BUF_LLEN 256 // 长缓冲区

/* Private Types ----------------------------------------------------------- */

// USART 配置索引请至 types.h 修改

// USART 配置结构体
typedef struct
{
    u32 rccGpio;
    u32 rccUart;
    GPIO_TypeDef *gpio;
    USART_TypeDef *uart;
    enum IRQn irqn;
    u16 tx;
    u16 rx;
} srl_s;

/* Global Variables -------------------------------------------------------- */

extern u8 srlReFlag;

extern __IO u8 srlSigBuf[];
extern __IO u8 srlSigFlg;
extern __IO u8 srlPidBuf[];
extern __IO u8 srlPidFlg;
extern __IO u8 srlCmdBuf[];
extern __IO u8 srlCmdFlg;
extern __IO u8 srlPkgBuf[];
extern __IO u8 srlPkgFlg;

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void serial_init(srl_e idx, u32 baudRate, u8 priority);

// 发送函数

void serial_send_byte(srl_e idx, u8 byte);
void serial_send_string(srl_e idx, u8 *str);
void serial_printf(srl_e idx, u8 *format, ...);

#endif

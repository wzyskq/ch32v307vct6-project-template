#ifndef __SERIAL_H
#define __SERIAL_H

#include "main.h"

/* -------------------------------- Exported Variables */

extern volatile u8 srlSigBuf[];
extern volatile u8 srlSigFlg;
extern volatile u8 srlPidBuf[];
extern volatile u8 srlPidFlg;
extern volatile u8 srlCmdBuf[];
extern volatile u8 srlCmdFlg;
extern volatile u8 srlPkgBuf[];
extern volatile u8 srlPkgFlg;

/* -------------------------------- Exported Functions */

/* 初始化函数 */

void serial_init(u8 serialNum, u32 baudRate, u8 subPriority);

/* 发送函数 */

void serial_send_byte(USART_TypeDef *USARTx, u8 Byte);
void serial_send_string(USART_TypeDef *USARTx, u8 *String);
void serial_printf(USART_TypeDef *USARTx, u8 *format, ...);

/* 处理函数 */

void serial_decode_sig(void);
void serial_decode_pid(void);
void serial_decode_cmd(void);
void serial_decode_pkg(void);

/* 等待函数 */

// u8 serial_wait_if(u8 *flagString, u8 (*getFlagFun)(void));
// void serial_wait_while(u8 *flagString, u8 (*getFlagFun)(void));

/* 获取标志位函数 */
// u8 get_sign_x_flag(void);
// u8 get_sign_d_flag(void);

#endif

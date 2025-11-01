#ifndef __SERIAL_H
#define __SERIAL_H

#include "main.h"

/* Exported Variables -------------------------------- */

extern volatile u8 serialSignBuf[];
extern volatile u8 serialSignOk;
extern volatile u8 serialPacketBuf[];
extern volatile u8 serialPacketOk;
extern volatile u8 serialPIDBuf[];
extern volatile u8 serialPIDOk;
extern volatile u8 serialCMDBuf[];
extern volatile u8 serialCMDOk;

/* Exported Functions -------------------------------- */

/* 初始化函数 */
void serial_init(u8 serialNum, u32 baudRate, u8 subPriority);

/* 发送函数 */
void serial_send_byte(USART_TypeDef *USARTx, u8 Byte);
void serial_send_string(USART_TypeDef *USARTx, u8 *String);
void serial_printf(USART_TypeDef *USARTx, u8 *format, ...);

/* 处理函数 */
// void serial_process_sign(void);
// void serial_process_packet(void);
// void serial_process_pid(void);
// void serial_process_cmd(void);

/* 等待函数 */

// u8 serial_wait_if(u8 *flagString, u8 (*getFlagFun)(void));
// void serial_wait_while(u8 *flagString, u8 (*getFlagFun)(void));

/* 获取标志位函数 */
// u8 get_sign_x_flag(void);
// u8 get_sign_d_flag(void);

#endif

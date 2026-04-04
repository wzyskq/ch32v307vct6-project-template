/******************************************************************
 ** \file    zdt_api.c
 **
 ** \author  Yiiry
 **
 ** \date    2026-04
 **
 ** \brief  本文件（zdt_api.c/.h）主要功能为处理电机返回数据包的接收、解析
 **
 ** \pre    需配合 Emm_V5.c/.h 使用
 **
 ** \note   张大头闭环步进电机本身就是集成好的闭环系统，一般情况下直接用 Emm_V5.c/.h 提供的命令控制电机即可.
 **         但如果需要对电机返回的数据包进行分析处理就比较麻烦，所以笔者就封装了这一套 API 来处理电机返回数据包的接收和解析.
 **         主要包括如下三个函数：
 **
 **         - zdt_irqHandler() 串口中断中调用，缓存命令.
 **
 **         - zdt_irqEndHandler() 定时中断中调用，终止缓存命令，最大超时时长由 **定时中断时基** 和 **超时判断周期** 两者乘积决定.
 **
 **         - Emm_V5_Get_Sys_Params() 定时中断中调用，解析缓存并传入的 zdtSysData 指针.
 **
 **         发送数据前已做阻塞判断处理，但建议在每次调用 Emm_V5 命令前先判断 zdtTxFlg 状态来避免数据冲突.
 **         判断状态方式可采用短时阻塞（逻辑清晰但浪费部分处理资源）或跳出逻辑（系统调度及时但复杂化上下逻辑）两种方式.
 **         具体示例如下.
 **
 ** \example  // 判断状态方式一：短时阻塞
 **             while (zdtTvFlg); // 等待清零
 **             Emm_V5_Vel_Control(ZDT_SRL, 1, 0, 60, 0, false);
 **             while (zdtTvFlg); // 等待清零
 **             Emm_V5_Vel_Control(ZDT_SRL, 1, 0, 30, 0, false);
 **
 ** \example  // 判断状态方式二：跳出逻辑
 **             if (!zdtTvFlg) {
 **                 Emm_V5_Vel_Control(ZDT_SRL, 1, 0, 60, 0, false);
 **                 // ... 其他命令 ...
 **             } else return; // 或者其他处理方式
 **
 ** \example  // 获取实时数据
 **             Emm_V5_Read_Sys_Params(ZDT_SRL, 1, S_VEL);                     // 申请获取速度
 **             while (zdtTvFlg);                                              // 等待更新数据
 **             serial_printf(SRL_RESRL, "v = %d rpm\n", (s32)zdtSysData.vel); // 串口打印数据
 **
 ** \note   Tips:
 **         - 减小定时器时基和最大超时周期可以提高数据接收的实时性，但过小也可能会增加丢包风险.
 */

#ifndef __ZDT_API_H
#define __ZDT_API_H

#include "main.h"
#include "Emm_V5.h"

/* Global Macros ----------------------------------------------------------- */

#define ZDT_NUM           2      // 电机数量
#define ZDT_SRL           usart3 // 电机串口号
#define ZDT_RESRL         usart1 // 调试返回串口号
#define ZDT_TIMEOUT       8      // 接收超时时长（ms）
#define ZDT_RVBUF_LEN     64     // 缓存数据包长度
#define ZDT_IRQEND_PERIOD 1      // 结束接收超时判断周期

/* Private Types ----------------------------------------------------------- */

typedef struct
{
    float vbus;  // 读取总线电压
    float cbus;  // 读取总线电流
    float cpha;  // 读取相电流
    float enco;  // 读取编码器原始值
    float clkc;  // 读取实时脉冲数
    float encl;  // 读取经过线性化校准后的编码器值
    float clki;  // 读取输入脉冲数
    float tpos;  // 读取电机目标位置
    float spos;  // 读取电机实时设定的目标位置
    float vel;   // 读取电机实时转速
    float cpos;  // 读取电机实时位置
    float perr;  // 读取电机位置误差
    float vbat;  // 读取多圈编码器电池电压（Y42）
    float temp;  // 读取电机实时温度（Y42）
    float flag;  // 读取电机状态标志位
    float oflag; // 读取回零状态标志位
    float oaf;   // 读取电机状态标志位 + 回零状态标志位（Y42）
    float pin;   // 读取引脚状态（Y42）
} zdtSysData_t;

/* Global Variables -------------------------------------------------------- */

// 存储

extern zdtSysData_t zdtSysData;

// 调试 & 安全

extern __IO bool zdtReFlag;
extern __IO bool zdtTimeoutFlg;
extern __IO u32 zdtTimeoutCnt;

// 接收 & 处理

extern __IO u8 zdtRxIdx;
extern __IO u8 zdtTimCnt;
extern __IO bool zdtTimFlg;
extern __IO bool zdtRvFlg;
extern __IO bool zdtTvFlg;
extern __IO u8 zdtRvBuf[];
extern u8 zdtTvTag[];

/* Global Functions -------------------------------------------------------- */

// 工具函数

void zdt_srl_send_hexString(srl_e idx, __IO u8 *data);
void zdt_irqTimeoutHandler(void);

// 接收函数

void zdt_irqHandler(USART_TypeDef *usart);
void zdt_irqEndHandler(void);

// 处理函数

void Emm_V5_Get_Sys_Params(zdtSysData_t *data);

#endif

#ifndef __MAIN_H
#define __MAIN_H

/* -------------------------------- System Includes */

#include "ch32v30x.h"
#include "ch32v30x_conf.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* -------------------------------- Exported Macro */

#define SRL_TIMEOUT  200 // 超时限制（单位：10ms）
#define SRL_BUF_SLEN 8   // 短缓冲区
#define SRL_BUF_MLEN 16  // 中缓冲区
#define SRL_BUF_LLEN 256 // 长缓冲区

/* -------------------------------- Exported Typedefs */

/* -------------------------------- User Includes */

#include "funs.h"
#include "timer.h"
#include "serial.h"
#include "led.h"
#include "servo.h"

/* -------------------------------- Exported Variable */

extern u8 infoFlag;  // 信息标志
extern u8 debugFlag; // 调试标志

/* -------------------------------- Exported Functions */

#endif /* __MAIN_H */
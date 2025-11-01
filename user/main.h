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

#define SERIAL_TIMEOUT     200 // 超时限制（单位：10ms）
#define SERIAL_BUF_SIZE    8   // 小缓冲区大小
#define SERIAL_BUFFER_SIZE 32  // 大缓冲区大小

/* -------------------------------- Exported Typedefs */

/* -------------------------------- User Includes */

#include "led.h"
#include "serial.h"

/* -------------------------------- Exported Variable */

/* -------------------------------- Exported Functions */

#endif /* __MAIN_H */
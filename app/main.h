#ifndef __MAIN_H
#define __MAIN_H

/* System Includes --------------------------------------------------------- */

#include "ch32v30x.h"
#include "types.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

/* Global Macros ----------------------------------------------------------- */

// 常用工具宏

#define ABS(x)    ((x) > 0 ? (x) : -(x))  // 伪泛型绝对值
#define MIN(a, b) ((a) < (b) ? (a) : (b)) // 伪泛型最小值
#define MAX(a, b) ((a) > (b) ? (a) : (b)) // 伪泛型最大值

/* Private Types ----------------------------------------------------------- */

/* User Includes ----------------------------------------------------------- */

// 业务及工具

#include "irq.h"
#include "sys.h"
#include "task.h"
#include "common.h"
#include "misc.h"

// 常用外设

#include "key.h"
#include "led.h"

// 三方组件
//
// ...

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

#endif

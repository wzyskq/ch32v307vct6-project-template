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

/* -------------------------------- Exported Typedefs */

typedef enum {
    LOW = 0,
    HIGH = 1
} level_t;

/* -------------------------------- User Includes */

#include "funs.h"
#include "timer.h"
#include "serial.h"
#include "led.h"
#include "oled.h"
#include "servo.h"
#include "sys.h"

/* -------------------------------- Exported Variable */

/* -------------------------------- Exported Functions */

#endif /* __MAIN_H */
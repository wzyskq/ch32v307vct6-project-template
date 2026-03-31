#ifndef __SERVO_H
#define __SERVO_H

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

void servo_set_spd(tim_e idx, u8 chNum, s16 spd);
void servo_set_pos(tim_e idx, u8 chNum, u8 pos);

#endif

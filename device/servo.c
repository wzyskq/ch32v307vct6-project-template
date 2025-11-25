#include "servo.h"

/*
    使用下面的函数前，请先调用 timer_pwmOut_init()
    初始化对应的定时器、通道和 PWM 参数（ARR、PSC）
*/

/* -------------------------------- Global Functions */

/******************************************************************
 * \brief      设置舵机速度
 * \param[in]  timNum 定时器编号 1~5、6~10
 * \param[in]  chNum 通道编号 1~4
 * \param[in]  spd 速度 -1000 ~ 1000
 * \note       理论上舵机受控范围为 0.5ms ~ 2.5ms（对应占空比 2.5% ~ 12.5%）
 *             但是实际我拿到的舵机（360°）受控范围为 0.5ms ~ 1.5ms（对应占空比 2.5% ~ 7.5%）
 */
void servo_set_spd(u8 timNum, u8 chNum, s16 spd)
{
    spd      = (spd < -1000) ? -1000 : spd;
    spd      = (spd > 1000) ? 1000 : spd; // 限幅
    u16 duty = 500 - spd / 4;             // 转换为 250~750 范围内的值
    timer_pwmOut_setDuty(timNum, chNum, duty);
}

/******************************************************************
 * \brief      设置舵机位置
 * \param[in]  timNum 定时器编号 1~5、6~10
 * \param[in]  chNum 通道编号 1~4
 * \param[in]  pos 位置 0 ~ 180
 */
void servo_set_pos(u8 timNum, u8 chNum, u8 pos)
{
    pos      = (pos > 180) ? 180 : pos; // 限幅
    u16 duty = 250 + (u16)pos * 50 / 9; // 转换为 250~1250 范围内的值
    timer_pwmOut_setDuty(timNum, chNum, duty);
}

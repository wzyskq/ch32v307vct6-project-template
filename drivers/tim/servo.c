#include "servo.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief      设置舵机速度
 * \param[in]  idx TIM 索引 1..5, 6..10
 * \param[in]  chNum 通道编号 1..4
 * \param[in]  spd 速度 -1000..1000
 * \note       请确保相关 TIM 和 GPIO 已经初始化
 * \note       理论上舵机受控范围为 0.5ms..2.5ms（对应占空比 2.5%..12.5%）
 *             但是实际我拿到的舵机（360°）受控范围为 0.5ms..1.5ms（对应占空比 2.5%..7.5%）
 */
void servo_set_spd(tim_e idx, u8 chNum, s16 spd)
{
    spd      = (spd < -1000) ? -1000 : spd;
    spd      = (spd > 1000) ? 1000 : spd; // 限幅
    u16 duty = 500 - spd / 4;             // 转换为 250~750 范围内的值
    timer_pwmOut_setDuty(idx, chNum, duty);
}

/******************************************************************
 * \brief      设置舵机位置
 * \param[in]  idx TIM 索引 1..5, 6..10
 * \param[in]  chNum 通道编号 1..4
 * \param[in]  pos 位置 0..180
 * \note       请确保相关 TIM 和 GPIO 已经初始化
 */
void servo_set_pos(tim_e idx, u8 chNum, u8 pos)
{
    pos      = (pos > 180) ? 180 : pos; // 限幅
    u16 duty = 250 + (u16)pos * 50 / 9; // 转换为 250..1250 范围内的值
    timer_pwmOut_setDuty(idx, chNum, duty);
}

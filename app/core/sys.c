#include "sys.h"

/* Private Macros ---------------------------------------------------------- */

#define SECBASE 100 // 秒基数，单位为 10ms

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

__IO u32 sysTime = 0; // 系统时间，单位为 10ms
__IO s16 whlCnt  = 0; // 轮子计数值

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief  系统主函数
 * \note   可在此函数内编写自己的程序代码
 */
void loop(void)
{
    while (1) {
        misc_srlParse_sig();
        misc_srlParse_pkg();
        misc_srlParse_pid();
        misc_srlParse_cmd();

        if (key_read(key1))
            led_turn(led1);

        // delay_ms(500);
        // leds_turn(1, 2);
        // delay_ms(500);
        // leds_turn(1, 2);
        // ++sysTime;
        // serial_printf(usart1, "...\r\n");

        // oled_printf(0, 0, OLED_8X16, "System Init!");
        oled_clear();
        oled_printf(0, 0, OLED_6X8, "prd: %5d", timPwmInData.period);
        oled_printf(8 * 8, 0, OLED_6X8, "hig: %5d", timPwmInData.high);
        oled_printf(0, 16, OLED_6X8, "frq: %3dHz", timPwmInData.freq);
        u8 dutyInt = timPwmInData.duty / 100;
        u8 dutyDec = timPwmInData.duty % 100;
        oled_printf(8 * 8, 16, OLED_6X8, "dty: %2d.%02d", dutyInt, dutyDec);

        oled_printf(8 * 4, 16 * 3, OLED_8X16, "%9d.%02d", sysTime / SECBASE, sysTime % SECBASE);
        oled_update();
    }
}

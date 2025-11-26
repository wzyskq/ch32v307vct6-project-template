#include "sys.h"

/* -------------------------------- Global Variable */

/* -------------------------------- Global Functions */

/******************************************************************
 * \brief  系统主函数
 * \note   可在此函数内编写自己的程序代码
 */
void system_loop(void)
{
    led_toggle(1);

    while (1) {
        serial_decode_sig();
        serial_decode_pkg();
        serial_decode_pid();
        serial_decode_cmd();

        delay_ms(500);
        leds_toggle(1, 2);
        delay_ms(500);
        leds_toggle(1, 2);
        // serial_printf(USART2, "...\r\n");

        oled_printf(0, 0, OLED_8X16, "System Init!");
        oled_update();
    }
}

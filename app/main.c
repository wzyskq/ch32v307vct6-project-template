#include "main.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 主程序 ******************** */

/******************************************************************
 * \brief   主程序
 * \note    本项目基于 CH32V307VCT6 评估板进行开发
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();

    delay_init();
    leds_init();
    keys_init();

    loop();
}

/* ******************** 主程序 */

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
    oled_init();

    serial_init(usart1, 115200, 31); // USART1 初始化
    // serial_init(usart2, 115200, 1); // USART2 初始化
    serial_init(usart3, 115200, 01); // USART3 初始化

    exter_init(exti2, 20); // EXTI2 初始化

    timer_init(timMode_timer, tim6, 14400, 5, 00);     // TIM6, 0.5ms
    timer_init(timMode_timer, tim7, 14400, 100, 11);   // TIM7, 10ms
    timer_init(timMode_pwmOut, tim10, 14400, 200, 33); // TIM10, 50Hz PWM 输出
    timer_init(timMode_pwmIn, tim3, 0, 144, 33);       // TIM3, PWM 输入接口
    // timer_init(timMode_encoder, tim3, 0, 0, 33);       // TIM3, 编码器接口

    serial_printf(usart1, "USART1: online.\n");

    // oled_printf(0, 0, OLED_8X16, "System Init!");

    loop();
}

/* ******************** 主程序 */

/*





*/

// /* 接口重映射 ******************** */
//
// /******************************************************************
//  * \brief      RCC_APB2Periph_GPIOx 重映射
//  * \param[in]  port 端口号，port=A..E
//  * \retval     RCC_APB2Periph_GPIOx 实际值
//  */
// u32 rcc_gpio_port(port_e port)
// {
//     return (u32)((u32)0x02U << (port));
// }
//
// /******************************************************************
//  * \brief      GPIO_TypeDef 重映射
//  * \param[in]  port 端口号，port=A..E
//  * \retval     GPIO_TypeDef 实际地址
//  */
// GPIO_TypeDef *gpio_port(port_e port)
// {
//     return (GPIO_TypeDef *)((u32)(APB2PERIPH_BASE + ((u32)port << 10) + 0x0400));
// }
//
// /******************************************************************
//  * \brief      GPIO_Pin_x 重映射
//  * \param[in]  pin 引脚号，pin=0..15
//  * \retval     GPIO_Pin_x 实际值
//  */
// u16 gpio_pin(u8 pin)
// {
//     if (pin > 15)
//         return 0;
//     return (u16)((u16)0x01U << (pin));
// }
//
// /* ******************** 接口重映射 */

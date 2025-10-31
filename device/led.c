#include "led.h"

static const u32 ledGpioRcc[2]      = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA};
static const u16 ledGpioPin[2]      = {GPIO_Pin_0, GPIO_Pin_1};
static GPIO_TypeDef *ledGpioPort[2] = {GPIOA, GPIOA};

/* */

/******************************************************************
 * \brief  初始化 LED
 * \param  ledNum LED 编号
 *   \arg  1, 2
 * \note   使用前请确保已经将对应的 GPIO 口连接到 LED1/LED2
 */
void led_init(u8 ledNum)
{
    ledNum += (ledNum > 0) ? -1 : 0; // 限幅转换为数组下标
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(ledGpioRcc[ledNum], ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ledGpioPin[ledNum];
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/******************************************************************
 * \brief  初始化所有 LED
 * \note   使用前请确保已经将对应的 GPIO 口连接到 LED1/LED2
 */
void leds_init(void)
{
    for (u8 i = 0; i < 2; i++)
        led_init(i + 1);
}

/******************************************************************
 * \brief  开启 LED
 * \param  ledNum LED 编号
 *   \arg  1, 2
 * \note   板载 LED 为低电平点亮
 */
void led_on(u8 ledNum)
{
    ledNum += (ledNum > 0) ? -1 : 0; // 限幅转换为数组下标
    GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_RESET);
}

/******************************************************************
 * \brief  关闭 LED
 * \param  ledNum LED 编号
 *   \arg  1, 2
 * \note   板载 LED 为高电平熄灭
 */
void led_off(u8 ledNum)
{
    ledNum += (ledNum > 0) ? -1 : 0; // 限幅转换为数组下标
    GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_SET);
}

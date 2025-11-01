#include "led.h"

/* -------------------------------- Static Variables */

static const u32 ledRccGpio[3]      = {0, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA};
static const u16 ledGpioPin[3]      = {0, GPIO_Pin_0, GPIO_Pin_1};
static GPIO_TypeDef *ledGpioPort[3] = {0, GPIOA, GPIOA};

/* -------------------------------- Global Functions */

/******************************************************************
 * \brief  初始化 LED
 * \param  ledNum LED 编号
 *   \arg  1, 2
 * \note   使用前请确保已经将对应的 GPIO 口连接到 LED1/LED2
 */
void led_init(u8 ledNum)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(ledRccGpio[ledNum], ENABLE);
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
    for (u8 i = 1; i <= 2; i++)
        led_init(i);
}

/******************************************************************
 * \brief  开启 LED
 * \param  ledNum LED 编号
 *   \arg  1, 2
 * \note   板载 LED 为低电平点亮
 */
void led_on(u8 ledNum)
{
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
    GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_SET);
}

/******************************************************************
 * \brief  切换 LED 状态
 * \param  ledNum LED 编号
 *   \arg  1, 2
 * \note   板载 LED 为低电平点亮
 */
void led_toggle(u8 ledNum)
{
    // 检测当前是否为熄灭状态
    if (GPIO_ReadOutputDataBit(ledGpioPort[ledNum], ledGpioPin[ledNum]) == Bit_SET)
        led_on(ledNum);
    else
        led_off(ledNum);
}

/******************************************************************
 * \brief  切换指定范围 LED 状态
 * \param  ledBeginNum 起始 LED 编号
 * \param  ledEndNum 结束 LED 编号
 * \note   板载 LED 为低电平点亮
 */
void leds_toggle(u8 ledBeginNum, u8 ledEndNum)
{
    for (u8 i = ledBeginNum; i <= ledEndNum; i++)
        led_toggle(i);
}

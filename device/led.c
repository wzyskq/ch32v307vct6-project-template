#include "led.h"

/* -------------------------------- Static Macro */

#define LED_NUM 2   // LED 数量
#define LED_ON  LOW // LED 点亮电平

/* -------------------------------- Static Constants */

static const u32 ledRccGpio[LED_NUM + 1]      = {0, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA};
static const u16 ledGpioPin[LED_NUM + 1]      = {0, GPIO_Pin_0, GPIO_Pin_1};
static GPIO_TypeDef *ledGpioPort[LED_NUM + 1] = {0, GPIOA, GPIOA};

/* -------------------------------- Global Functions */

/******************************************************************
 * \brief      初始化 LED
 * \param[in]  ledNum LED 编号
 *   \arg      1, 2, ...
 * \note       使用前请确保已经将对应的 GPIO 口连接到对应的 LED
 */
void led_init(u8 ledNum)
{
    if (ledNum < 1 || LED_NUM < ledNum) return;

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(ledRccGpio[ledNum], ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ledGpioPin[ledNum];
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ledGpioPort[ledNum], &GPIO_InitStructure);

    led_off(ledNum);
}

/******************************************************************
 * \brief  初始化所有 LED
 * \note   使用前请确保已经将对应的 GPIO 口连接到对应的 LED
 */
void leds_init(void)
{
    for (u8 i = 1; i <= LED_NUM; i++)
        led_init(i);
}

/******************************************************************
 * \brief      开启 LED
 * \param[in]  ledNum LED 编号
 *   \arg      1, 2, ...
 */
void led_on(u8 ledNum)
{
    if (ledNum < 1 || LED_NUM < ledNum) return;

#if (LED_ON == LOW)
    GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_RESET);
#else
    GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_SET);
#endif
}

/******************************************************************
 * \brief      关闭 LED
 * \param[in]  ledNum LED 编号
 *   \arg      1, 2, ...
 */
void led_off(u8 ledNum)
{
    if (ledNum < 1 || LED_NUM < ledNum) return;

#if (LED_ON == LOW)
    GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_SET);
#else
    GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_RESET);
#endif
}

/******************************************************************
 * \brief      切换 LED 状态
 * \param[in]  ledNum LED 编号
 *   \arg      1, 2, ...
 */
void led_toggle(u8 ledNum)
{
    if (ledNum < 1 || LED_NUM < ledNum) return;

    // 检测当前是否为熄灭状态
    if (GPIO_ReadOutputDataBit(ledGpioPort[ledNum], ledGpioPin[ledNum]) == Bit_SET)
        GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_RESET);
    else
        GPIO_WriteBit(ledGpioPort[ledNum], ledGpioPin[ledNum], Bit_SET);
}

/******************************************************************
 * \brief      切换指定范围 LED 状态
 * \param[in]  ledBeginNum 起始 LED 编号
 * \param[in]  ledEndNum 结束 LED 编号
 */
void leds_toggle(u8 ledBeginNum, u8 ledEndNum)
{
    for (u8 i = ledBeginNum; i <= ledEndNum; i++)
        led_toggle(i);
}

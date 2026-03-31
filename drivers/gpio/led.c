#include "led.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const led_s ledList[] = {
    [led1] = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_0, low}, // PA0
    [led2] = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_1, low}, // PA1
};

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      初始化 LED
 * \param[in]  idx LED 索引
 * \note       请确保私有量 ledList 正确配置及 ledNum 置于配置索引末尾
 */
void led_init(led_e idx)
{
    GPIO_InitTypeDef gpio = {0};
    RCC_APB2PeriphClockCmd(ledList[idx].rccGpio, ENABLE);
    gpio.GPIO_Pin   = ledList[idx].pin;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ledList[idx].gpio, &gpio);

    led_off(idx);
}

/******************************************************************
 * \brief  初始化所有 LED
 * \note   请先确保 ledNum 置于配置索引末尾
 */
void leds_init(void)
{
    for (u8 i = 0; i < ledNum; i++)
        led_init(i);
}

/* ******************** 初始化函数 */

/*





*/

/* 操作函数 ******************** */

/******************************************************************
 * \brief      开启指定 LED
 * \param[in]  idx LED 索引
 */
void led_on(u8 idx)
{
    if (idx < 0 || idx >= ledNum) return;
    GPIO_WriteBit(ledList[idx].gpio, ledList[idx].pin, (BitAction)ledList[idx].ledOnLevel);
}

/******************************************************************
 * \brief      关闭指定 LED
 * \param[in]  idx LED 索引
 */
void led_off(u8 idx)
{
    if (idx < 0 || idx >= ledNum) return;
    GPIO_WriteBit(ledList[idx].gpio, ledList[idx].pin, (BitAction)!ledList[idx].ledOnLevel);
}

/******************************************************************
 * \brief      切换指定 LED 状态
 * \param[in]  idx LED 索引
 */
void led_turn(u8 idx)
{
    if (idx < 0 || idx >= ledNum) return;

    // 检测当前是否为熄灭状态
    if (GPIO_ReadOutputDataBit(ledList[idx].gpio, ledList[idx].pin) == Bit_SET)
        GPIO_WriteBit(ledList[idx].gpio, ledList[idx].pin, Bit_RESET);
    else
        GPIO_WriteBit(ledList[idx].gpio, ledList[idx].pin, Bit_SET);
}

/******************************************************************
 * \brief      切换指定范围 LED 状态
 * \param[in]  begIdx 起始 LED 索引
 * \param[in]  endIdx 结束 LED 索引
 */
void leds_turn(u8 begIdx, u8 endIdx)
{
    for (u8 i = begIdx; i <= endIdx; i++)
        led_turn(i);
}

/* ******************** 操作函数 */

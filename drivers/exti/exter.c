#include "exter.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const ext_s extList[] = {
    [exti2] = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_2, GPIO_PortSourceGPIOA, GPIO_PinSource2, EXTI_Line2, EXTI_Trigger_Falling, EXTI2_IRQn},
};

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      初始化指定 EXTI
 * \param[in]  idx EXTI 索引
 * \param[in]  priority 优先级，格式：抢占优先级+响应优先级. 例：01 表示抢占优先级0，响应优先级1
 * \note       请确保私有量 extList 已正确配置
 */
void exter_init(ext_e idx, u8 priority)
{
    /* 时钟使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | extList[idx].rccGpio, ENABLE);

    /* GPIO 配置 */
    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Pin         = extList[idx].pin;
    gpio.GPIO_Mode        = GPIO_Mode_IPU;
    GPIO_Init(extList[idx].gpio, &gpio);

    /* AFIO 配置 */
    GPIO_EXTILineConfig(extList[idx].portSrc, extList[idx].pinSrc);

    /* EXTI 配置 */
    EXTI_InitTypeDef exti = {0};
    exti.EXTI_Line        = extList[idx].extiLine;
    exti.EXTI_Mode        = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger     = extList[idx].trigger; // 触发边沿
    exti.EXTI_LineCmd     = ENABLE;
    EXTI_Init(&exti);

    /* NVIC 配置 */
    NVIC_InitTypeDef nvic                  = {0};
    nvic.NVIC_IRQChannel                   = extList[idx].irqn;
    nvic.NVIC_IRQChannelPreemptionPriority = PreemptingPriority(priority);
    nvic.NVIC_IRQChannelSubPriority        = SubPriority(priority);
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

/* ******************** 初始化函数 */

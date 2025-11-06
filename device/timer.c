#include "timer.h"

/* -------------------------------------------------- Static Constants */

static const u32 timRccTim[]  = {0, RCC_APB2Periph_TIM1, RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4, RCC_APB1Periph_TIM5, RCC_APB1Periph_TIM6, RCC_APB1Periph_TIM7, RCC_APB2Periph_TIM8, RCC_APB2Periph_TIM9, RCC_APB2Periph_TIM10};
static const u32 timRccGpio[] = {0, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOA, 0, 0, RCC_APB2Periph_GPIOC, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC};

static GPIO_TypeDef *timGpioPort[][4] = {
    {0, 0, 0, 0},
    {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM1: PA8,9,10,11
    {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM2: PA0,1,2,3
    {GPIOA, GPIOA, GPIOB, GPIOB}, // TIM3: PA6,7, PB0,1
    {GPIOB, GPIOB, GPIOB, GPIOB}, // TIM4: PB6,7,8,9
    {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM5: PA0,1,2,3

    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {GPIOC, GPIOC, GPIOC, GPIOC}, // TIM8: PC6,7,8,9
    {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM9: PA2,3,4,5
    {GPIOB, GPIOB, GPIOC, GPIOC}  // TIM10: PB8,9, PC3,11
};

static TIM_TypeDef *timTimePort[] = {0, TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8, TIM9, TIM10};

static const u16 timGpioPin[][4] = {
    {0, 0, 0, 0},
    {GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11}, // TIM1
    {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3},   // TIM2
    {GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_0, GPIO_Pin_1},   // TIM3
    {GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9},   // TIM4
    {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3},   // TIM5
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9}, // TIM8
    {GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5}, // TIM9
    {GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_3, GPIO_Pin_11} // TIM10
};

/* -------------------------------------------------- Global Functions */

/******************************************************************
 * \brief PWM 输出初始化
 * \param timNum 定时器编号 1~5、6~10
 * \param chNum 通道编号 1~4
 * \param arr 自动重装载值 + 1
 * \param psc 预分频值 + 1
 */
void timer_pwmOut_init(u8 timNum, u8 chNum, u16 arr, u16 psc)
{
    if (timNum <= 0 || timNum == 6 || timNum == 7 || timNum >= 11)
        return;

    if (timNum == 1 || timNum >= 8)
        RCC_APB2PeriphClockCmd(timRccTim[timNum], ENABLE);
    else
        RCC_APB1PeriphClockCmd(timRccTim[timNum], ENABLE);
    RCC_APB2PeriphClockCmd(timRccGpio[timNum], ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin         = timGpioPin[timNum][chNum - 1];
    GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
    GPIO_Init(timGpioPort[timNum][chNum - 1], &GPIO_InitStructure);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_TimeBaseInitStructure.TIM_Period              = arr - 1;            // 自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler           = psc - 1;            // 预分频值
    TIM_TimeBaseInitStructure.TIM_ClockDivision       = TIM_CKD_DIV1;       // 时钟分割
    TIM_TimeBaseInitStructure.TIM_CounterMode         = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(timTimePort[timNum], &TIM_TimeBaseInitStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_OCInitStructure.TIM_OCMode        = TIM_OCMode_PWM1;        // 模式1：当计数器计数值<CCRx时，输出高电平，模式2反之
    TIM_OCInitStructure.TIM_OutputState   = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse         = 0;                      // 初始占空比为0
    TIM_OCInitStructure.TIM_OCPolarity    = TIM_OCPolarity_High;    // 输出极性，高电平有效
    // TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    if (chNum == 1)
        TIM_OC1Init(timTimePort[timNum], &TIM_OCInitStructure);
    else if (chNum == 2)
        TIM_OC2Init(timTimePort[timNum], &TIM_OCInitStructure);
    else if (chNum == 3)
        TIM_OC3Init(timTimePort[timNum], &TIM_OCInitStructure);
    else if (chNum == 4)
        TIM_OC4Init(timTimePort[timNum], &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(timTimePort[timNum], ENABLE);
    // TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
    if (chNum == 1)
        TIM_OC1PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
    else if (chNum == 2)
        TIM_OC2PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
    else if (chNum == 3)
        TIM_OC3PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
    else if (chNum == 4)
        TIM_OC4PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(timTimePort[timNum], ENABLE);
    TIM_Cmd(timTimePort[timNum], ENABLE);
}

/******************************************************************
 * \brief 设置 PWM 占空比
 * \param timNum 定时器编号 1~5、8~10
 * \param chNum 通道编号 1~4
 * \param duty 占空比 0~10000 (0.00% ~ 100.00%)
 */
void timer_pwmOut_setDuty(u8 timNum, u8 chNum, u16 duty)
{
    if (timNum <= 0 || timNum == 6 || timNum == 7 || timNum >= 11)
        return;

    u16 arr = timTimePort[timNum]->ATRLR + 1;
    u16 pulse = (u32)duty * arr / 10000;

    if (chNum == 1)
        timTimePort[timNum]->CH1CVR = pulse;
    else if (chNum == 2)
        timTimePort[timNum]->CH2CVR = pulse;
    else if (chNum == 3)
        timTimePort[timNum]->CH3CVR = pulse;
    else if (chNum == 4)
        timTimePort[timNum]->CH4CVR = pulse;
}
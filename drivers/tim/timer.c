#include "timer.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

// 参考信息列表
//
// static const u32 timRccTim[] = {
//     [tim1]  = RCC_APB2Periph_TIM1,
//     [tim2]  = RCC_APB1Periph_TIM2,
//     [tim3]  = RCC_APB1Periph_TIM3,
//     [tim4]  = RCC_APB1Periph_TIM4,
//     [tim5]  = RCC_APB1Periph_TIM5,
//     [tim6]  = RCC_APB1Periph_TIM6,
//     [tim7]  = RCC_APB1Periph_TIM7,
//     [tim8]  = RCC_APB2Periph_TIM8,
//     [tim9]  = RCC_APB2Periph_TIM9,
//     [tim10] = RCC_APB2Periph_TIM10,
// };
//
// static const u32 timRccGpio[] = {
//     [tim1]  = RCC_APB2Periph_GPIOA,
//     [tim2]  = RCC_APB2Periph_GPIOA,
//     [tim3]  = RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,
//     [tim4]  = RCC_APB2Periph_GPIOB,
//     [tim5]  = RCC_APB2Periph_GPIOA,
//     [tim6]  = 0,
//     [tim7]  = 0,
//     [tim8]  = RCC_APB2Periph_GPIOC,
//     [tim9]  = RCC_APB2Periph_GPIOA,
//     [tim10] = RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,
// };
//
// static GPIO_TypeDef *timGpioPort[][4] = {
//     [tim1]  = {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM1: PA8,9,10,11
//     [tim2]  = {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM2: PA0,1,2,3
//     [tim3]  = {GPIOA, GPIOA, GPIOB, GPIOB}, // TIM3: PA6,7, PB0,1
//     [tim4]  = {GPIOB, GPIOB, GPIOB, GPIOB}, // TIM4: PB6,7,8,9
//     [tim5]  = {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM5: PA0,1,2,3
//     [tim6]  = {0, 0, 0, 0},                 // TIM6
//     [tim7]  = {0, 0, 0, 0},                 // TIM7
//     [tim8]  = {GPIOC, GPIOC, GPIOC, GPIOC}, // TIM8: PC6,7,8,9
//     [tim9]  = {GPIOA, GPIOA, GPIOA, GPIOA}, // TIM9: PA2,3,4,5
//     [tim10] = {GPIOB, GPIOB, GPIOC, GPIOC}  // TIM10: PB8,9, PC3,11
// };
//
// static const IRQn_Type timTimeIRQn[] = {
//     [tim1]  = TIM1_UP_IRQn,
//     [tim2]  = TIM2_IRQn,
//     [tim3]  = TIM3_IRQn,
//     [tim4]  = TIM4_IRQn,
//     [tim5]  = TIM5_IRQn,
//     [tim6]  = TIM6_IRQn,
//     [tim7]  = TIM7_IRQn,
//     [tim8]  = TIM8_UP_IRQn,
//     [tim9]  = TIM9_UP_IRQn,
//     [tim10] = TIM10_UP_IRQn,
// };

static const tim_s timList[] = {
    [tim1] = {0},
    [tim2] = {0},
    [tim3] = {
        RCC_APB2Periph_GPIOA,
        12,
        {GPIOA, GPIOA, 0, 0},
        {GPIO_Pin_6, GPIO_Pin_7, 0, 0},
        RCC_APB1Periph_TIM3,
        TIM3,
        0,
    },
    [tim4]  = {0},
    [tim5]  = {0},
    [tim6]  = {0, 0, {0}, {0}, RCC_APB1Periph_TIM6, TIM6, TIM6_IRQn}, // 基本定时器
    [tim7]  = {0, 0, {0}, {0}, RCC_APB1Periph_TIM7, TIM7, TIM7_IRQn}, // 基本定时器
    [tim8]  = {0},
    [tim9]  = {0},
    [tim10] = {
        RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,
        34,
        {0, 0, GPIOC, GPIOC},
        {0, 0, GPIO_Pin_3, GPIO_Pin_11},
        RCC_APB2Periph_TIM10,
        TIM10,
        TIM10_UP_IRQn,
    },
};

/* Global Variables -------------------------------------------------------- */

__IO timPwmInData_s timPwmInData; // PWM 输入测量结果

/* Private Functions ------------------------------------------------------- */

/******************************************************************
 * \brief      获取定时器时钟频率
 * \param[in]  idx 定时器编号 1..10
 * \retval     定时器时钟频率，单位 Hz
 */
static u32 timer_getClkHz(tim_e idx)
{
    RCC_ClocksTypeDef clocks = {0};
    RCC_GetClocksFreq(&clocks);

    if (idx == tim1 || idx >= tim8) {
        u32 clk = clocks.PCLK2_Frequency;
        if ((RCC->CFGR0 & RCC_PPRE2) != RCC_PPRE2_DIV1)
            clk <<= 1;
        return clk;
    }

    u32 clk = clocks.PCLK1_Frequency;
    if ((RCC->CFGR0 & RCC_PPRE1) != RCC_PPRE1_DIV1)
        clk <<= 1;
    return clk;
}

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      定时器初始化
 * \param[in]  mode 初始化模式
 *   \arg        timMode_timer，定时器模式
 *   \arg        timMode_pwmIn，PWM 输入模式
 *   \arg        timMode_pwmOut，PWM 输出模式
 *   \arg        timMode_encoder，编码器模式
 * \param[in]  idx 定时器编号 1..10
 * \param[in]  arr 自动重装载值 + 1
 * \param[in]  psc 预分频值 + 1
 * \param[in]  priority 优先级，格式：抢占优先级+响应优先级. 例：01 表示抢占优先级0，响应优先级1
 *
 * \retval     bool 初始化成功与否
 */
bool timer_init(timMode_e mode, tim_e idx, u16 arr, u16 psc, u8 priority)
{
    /* 基本参数检查 */
    if (idx < tim1 || idx > tim10)
        return false;
    if (timList[idx].tim == 0 || timList[idx].rccTim == 0)
        return false;

    /* 功能定时器检查 */
    if ((mode == timMode_pwmIn || mode == timMode_pwmOut || mode == timMode_encoder) && (idx == tim6 || idx == tim7))
        return false;

    /* 定时模式参数检查 */
    if ((mode == timMode_timer || mode == timMode_pwmOut) && (arr == 0 || psc == 0))
        return false;

    /* 解析通道号 */
    u8 chList[5] = {0};
    u16 chNum    = timList[idx].chNum;
    while (chNum) {
        u8 digit = chNum % 10;
        if (digit >= 1 && digit <= 4)
            chList[++chList[0]] = digit;
        chNum /= 10;
    }

    // pwmIn/pwmOut/encoder 模式至少需要一个通道，且 PWM 输入模式必须使用 CH1/CH2
    if ((mode == timMode_pwmIn || mode == timMode_pwmOut || mode == timMode_encoder) && chList[0] == 0)
        return false;

    // PWM 输入模式依赖 CH1/CH2
    if (mode == timMode_pwmIn && !(timList[idx].chNum == 12 || timList[idx].chNum == 21))
        return false;

    /* 时钟使能 */
    if (idx == tim1 || idx >= tim8)
        RCC_APB2PeriphClockCmd(timList[idx].rccTim, ENABLE); // 高级定时器
    else
        RCC_APB1PeriphClockCmd(timList[idx].rccTim, ENABLE); // 通用、基本定时器
    if (mode == timMode_pwmIn || mode == timMode_pwmOut || mode == timMode_encoder)
        RCC_APB2PeriphClockCmd(timList[idx].rccGpio, ENABLE); // GPIO 时钟

    TIM_DeInit(timList[idx].tim); // 复位定时器寄存器到默认值，避免之前的配置干扰

    /* GPIO 配置 */
    if (mode == timMode_pwmOut) {
        for (u8 i = 1; i <= chList[0]; i++) {
            GPIO_InitTypeDef gpio = {0};
            gpio.GPIO_Pin         = timList[idx].pin[chList[i] - 1];
            gpio.GPIO_Mode        = GPIO_Mode_AF_PP; // 复用推挽输出
            gpio.GPIO_Speed       = GPIO_Speed_50MHz;
            GPIO_Init(timList[idx].gpio[chList[i] - 1], &gpio);
        }
    } else if (mode == timMode_pwmIn || mode == timMode_encoder) {
        for (u8 i = 1; i <= chList[0]; i++) {
            GPIO_InitTypeDef gpio = {0};
            gpio.GPIO_Pin         = timList[idx].pin[chList[i] - 1];
            gpio.GPIO_Mode        = GPIO_Mode_IPU; // 上拉输入，采集 PWM/编码器信号
            gpio.GPIO_Speed       = GPIO_Speed_50MHz;
            GPIO_Init(timList[idx].gpio[chList[i] - 1], &gpio);
        }
    }

    /* 定时器配置 */
    TIM_TimeBaseInitTypeDef tb = {0};
    tb.TIM_Period              = (mode == timMode_encoder) ? 0xFFFF : (u16)(arr - 1); // encoder 模式固定全计数
    tb.TIM_Prescaler           = (mode == timMode_encoder) ? 0 : (u16)(psc - 1);      // encoder 模式不分频
    tb.TIM_ClockDivision       = TIM_CKD_DIV1;
    tb.TIM_CounterMode         = TIM_CounterMode_Up;
    TIM_TimeBaseInit(timList[idx].tim, &tb);

    /* TIM 定时模式中断配置 */
    if (mode == timMode_timer && timList[idx].irqn) {
        TIM_ClearITPendingBit(timList[idx].tim, TIM_IT_Update); // 清除首次更新标志

        NVIC_InitTypeDef nvic                  = {0};
        nvic.NVIC_IRQChannel                   = timList[idx].irqn;
        nvic.NVIC_IRQChannelPreemptionPriority = PreemptingPriority(priority); // 抢占优先级
        nvic.NVIC_IRQChannelSubPriority        = SubPriority(priority);        // 响应优先级
        nvic.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&nvic);

        TIM_ITConfig(timList[idx].tim, TIM_IT_Update, ENABLE); // 允许更新中断
    }

    // PWM 输入模式配置
    else if (mode == timMode_pwmIn) {
        TIM_ICInitTypeDef ic = {0};
        ic.TIM_Channel       = TIM_Channel_1; // 以 CH1 为基准，自动配对 CH2
        ic.TIM_ICPolarity    = TIM_ICPolarity_Rising;
        ic.TIM_ICSelection   = TIM_ICSelection_DirectTI;
        ic.TIM_ICPrescaler   = TIM_ICPSC_DIV1;
        ic.TIM_ICFilter      = 0x0A;

        TIM_PWMIConfig(timList[idx].tim, &ic);
        TIM_SelectInputTrigger(timList[idx].tim, TIM_TS_TI1FP1);
        TIM_SelectSlaveMode(timList[idx].tim, TIM_SlaveMode_Reset);
        TIM_SelectMasterSlaveMode(timList[idx].tim, TIM_MasterSlaveMode_Enable);

        TIM_ClearFlag(timList[idx].tim, TIM_FLAG_CC1 | TIM_FLAG_CC2 | TIM_FLAG_Update);
    }

    // PWM 输出模式配置
    else if (mode == timMode_pwmOut) {
        TIM_OCInitTypeDef oc = {0};
        oc.TIM_OCMode        = TIM_OCMode_PWM1;        // PWM 模式 1
        oc.TIM_OutputState   = TIM_OutputState_Enable; // 使能输出
        oc.TIM_OCPolarity    = TIM_OCPolarity_High;    // 高电平有效
        oc.TIM_Pulse         = 0;                      // 初始占空比 0

        for (u8 i = 1; i <= chList[0]; i++) {
            if (chList[i] == 1) {
                TIM_OC1Init(timList[idx].tim, &oc);
                TIM_OC1PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            } else if (chList[i] == 2) {
                TIM_OC2Init(timList[idx].tim, &oc);
                TIM_OC2PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            } else if (chList[i] == 3) {
                TIM_OC3Init(timList[idx].tim, &oc);
                TIM_OC3PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            } else if (chList[i] == 4) {
                TIM_OC4Init(timList[idx].tim, &oc);
                TIM_OC4PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            }
        }

        TIM_ARRPreloadConfig(timList[idx].tim, ENABLE); // 使能自动重装载缓冲
        TIM_CtrlPWMOutputs(timList[idx].tim, ENABLE);   // 高级定时器输出使能
    }

    // 编码器模式配置
    else if (mode == timMode_encoder) {
        TIM_EncoderInterfaceConfig(timList[idx].tim,
                                   TIM_EncoderMode_TI12,   // 默认1/2双通道编码器模式
                                   TIM_ICPolarity_Rising,  // CH1 上升沿
                                   TIM_ICPolarity_Rising); // CH2 上升沿

        TIM_ICInitTypeDef ic = {0};
        ic.TIM_ICPolarity    = TIM_ICPolarity_Rising;
        ic.TIM_ICSelection   = TIM_ICSelection_DirectTI;
        ic.TIM_ICPrescaler   = TIM_ICPSC_DIV1;
        ic.TIM_ICFilter      = 0x0A; // 输入滤波

        for (u8 i = 1; i <= chList[0]; i++) {
            if (chList[i] == 1) {
                ic.TIM_Channel = TIM_Channel_1;
                TIM_ICInit(timList[idx].tim, &ic);
            } else if (chList[i] == 2) {
                ic.TIM_Channel = TIM_Channel_2;
                TIM_ICInit(timList[idx].tim, &ic);
            } else if (chList[i] == 3) {
                ic.TIM_Channel = TIM_Channel_3;
                TIM_ICInit(timList[idx].tim, &ic);
            } else if (chList[i] == 4) {
                ic.TIM_Channel = TIM_Channel_4;
                TIM_ICInit(timList[idx].tim, &ic);
            }
        }

        TIM_SetCounter(timList[idx].tim, 0); // 编码器计数清零
    }

    TIM_Cmd(timList[idx].tim, ENABLE); // 启动定时器
    return true;
}

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief       读取 PWM 输入信息（轮询方式）
 * \param[in]   idx TIM 索引（需配置 CH1/CH2）
 * \param[out]  data PWM 输入测量结果
 * \retval      bool 是否读取成功
 */
bool timer_pwmIn_readData(tim_e idx, __IO timPwmInData_s *data)
{
    if (idx < tim1 || idx == tim6 || idx == tim7 || idx > tim10)
        return false;
    if (data == NULL)
        return false;

    u16 prd = TIM_GetCapture1(timList[idx].tim);
    u16 hig = TIM_GetCapture2(timList[idx].tim);
    if (prd == 0)
        return false;
    if (hig > prd)
        hig = prd;

    data->period = prd;
    data->high   = hig;
    data->duty   = (u16)roundf(hig * 10000.0f / prd);

    u32 timClkHz = timer_getClkHz(idx);
    u32 pscDiv   = (u32)TIM_GetPrescaler(timList[idx].tim) + 1U;
    data->freq   = (timClkHz == 0 || pscDiv == 0) ? 0 : (timClkHz / pscDiv / prd);

    return true;
}

/******************************************************************
 * \brief      设置 PWM 占空比
 * \param[in]  idx TIM 索引 1..5, 8..10
 * \param[in]  chNum 通道编号 1..4
 * \param[in]  duty 占空比 0..10000 (0.00%..100.00%)
 * \retval     bool 是否设置成功
 */
bool timer_pwmOut_setDuty(tim_e idx, u8 chNum, u16 duty)
{
    if (idx < tim1 || idx == tim6 || idx == tim7 || idx > tim10)
        return false;

    u16 arr   = timList[idx].tim->ATRLR + 1;
    u16 pulse = (u32)duty * arr / 10000;

    if (chNum == 1)
        timList[idx].tim->CH1CVR = pulse;
    else if (chNum == 2)
        timList[idx].tim->CH2CVR = pulse;
    else if (chNum == 3)
        timList[idx].tim->CH3CVR = pulse;
    else if (chNum == 4)
        timList[idx].tim->CH4CVR = pulse;

    return true;
}

/******************************************************************
 * \brief  读取编码器计数值
 * \param[in]  idx 定时器编号 1..5, 8..10
 * \param[in]  sign 计数值符号选择
 *   \arg       - normal: 正常计数值
 *   \arg       - inverse: 取反计数值
 * \retval     编码器计数值
 * \note       读取后会清零计数值
 */
s16 timer_encoder_readCnt(tim_e idx, sign_e sign)
{
    if (idx == tim7 || idx == tim6)
        return 0;

    // 将 uint32_t 强制转换为 int16_t 来正确处理负数
    s16 cnt = (s16)TIM_GetCounter(timList[idx].tim); // 读取计数值
    TIM_SetCounter(timList[idx].tim, 0);             // 清零

    return (sign) ? -cnt : cnt; // 选择性取反
}

/* ******************** 功能函数 */

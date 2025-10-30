#include "led.h"

void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void led_on(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
}

void led_off(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
}

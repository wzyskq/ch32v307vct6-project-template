#include "key.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const key_s keyList[] = {
    [key1] = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_2, low}, // PA2
};

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      初始化指定按键
 * \param[in]  idx 按键编号
 * \note       请确保私有量 keyList 正确配置及 keyNum 置于配置索引末尾
 */
void key_init(key_e idx)
{
    RCC_APB2PeriphClockCmd(keyList[idx].rccGpio, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = keyList[idx].pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = keyList[idx].keyOnLevel == high ? GPIO_Mode_IPD : GPIO_Mode_IPU; // 根据按下电平设置输入模式
    GPIO_Init(keyList[idx].gpio, &GPIO_InitStructure);
}

/******************************************************************
 * \brief  初始化所有按键
 * \note   请先确保 keyNum 置于配置索引末尾
 */
void keys_init(void)
{
    for (key_e i = 0; i < keyNum; i++)
        key_init(i);
}

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief      读取指定按键状态
 * \param[in]  idx 按键编号
 * \retval     bool 按键状态
 */
bool key_read(key_e idx)
{
    if (GPIO_ReadInputDataBit(keyList[idx].gpio, keyList[idx].pin) == keyList[idx].keyOnLevel) {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(keyList[idx].gpio, keyList[idx].pin) == keyList[idx].keyOnLevel);
        return true;
    }
    return false;
}

/******************************************************************
 * \brief   扫描按键
 * \return  key_e 按键索引，0xFF 表示无按键按下
 */
key_e key_scan(void)
{
    for (key_e i = 0; i < keyNum; i++) {
        if (key_read(i))
            return i;
    }
    return 0xFF; // 无按键按下
}

/* ******************** 功能函数 */

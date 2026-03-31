#include "key.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const key_s keyList[] = {
    [key1] = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_2, low}, // PA2
};

/* Global Variables -------------------------------------------------------- */

u16 keyBox[] = {0, 0, 0}; // 按键时间 {按键编号, 按键次数, 按键时间} 单位：10ms
u8 taskNum   = 0;         // 当前按键

/*





*/

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
    for (u8 i = 0; i < keyNum; i++)
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
 * \return  i 按键编号，0 表示无按键按下
 */
u8 key_scan(void)
{
    for (u8 i = 0; i < keyNum; i++) {
        if (key_read(i))
            return i;
    }
    return 0;
}

/******************************************************************
 * @brief  按键检测
 */
void key_judge(void)
{
    u8 keyst = key_scan();
    if (keyst) {
        if (keyst != keyBox[0] || keyBox[2] == 0) // 重置按键条件：按键不同 或 按键计时为零
        {
            for (u8 i = 0; i < 3; i++)
                keyBox[i] = 0;

            keyBox[0] = keyst;
            keyBox[1] = 1;
            keyBox[2] = 1; // 设置为 1，开启在定时中断中自增
        } else {
            keyBox[1]++;
        }

        if (keyBox[0]) {
            oled_printf(0, 48, OLED_8X16, "%d_%d", keyBox[0], keyBox[1]);
            // oled_update();
        }
    }
}

/******************************************************************
 * @brief  执行按键动作
 */
void key_action(void)
{
    if (keyBox[0] == 1) {
        if (keyBox[1] == 1)
            taskNum = 1;
        else if (keyBox[1] == 2)
            taskNum = 5;
    } else if (keyBox[0] == 2) {
        if (keyBox[1] == 1)
            taskNum = 2;
        else if (keyBox[1] == 2)
            taskNum = 6;
    } else if (keyBox[0] == 3) {
        if (keyBox[1] == 1)
            taskNum = 3;
        else if (keyBox[1] == 2)
            taskNum = 7;
    } else if (keyBox[0] == 4) {
        if (keyBox[1] == 1)
            taskNum = 4;
        else if (keyBox[1] == 2)
            taskNum = 8;
    }

    oled_printf(0, 48, OLED_8X16, "%d-%d", keyBox[0], keyBox[1]);
    // oled_update();

    if (keyBox[0])
        keyBox[0] = 0;
}

/* ******************** 功能函数 */

#include "irq.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* TIM 中断请求 ******************** */

/******************************************************************
 * \brief  TIM7 中断请求处理函数
 * \note   系统任务调度时基
 */
void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) {

        // ...你的代码...

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}

/* ******************** TIM 中断请求 */

/*





*/

/* USART 中断请求 ******************** */

/******************************************************************
 * @fn     USART1_IRQHandler
 * @brief  该函数处理 USART1 中断请求
 */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {

        // ...你的代码...

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/* ******************** USART 中断请求 */

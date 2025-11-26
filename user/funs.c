#include "funs.h"

/* -------------------------------- Global Functions */

/******************************************************************
 * \brief      长字符串模式匹配函数
 * \param[in]  src 源字符串
 * \param[in]  pattern 模式字符串
 * \return     如果匹配成功，返回模式字符串后的位置指针；否则返回 NULL
 * \note       - 忽略 src 前导空格，匹配 pattern 全部内容
 *             - 该函数适用于长度大于 10Bytes 的模式匹配
 */
u8 *strmatch_l(u8 *src, u8 *pattern)
{
    while (*src == ' ') // 跳过前导空格
        src++;
    size_t len = strlen(pattern);
    if (!strncmp(src, pattern, len)) return src + len;
    return NULL;
}

/******************************************************************
 * \brief      短字符串模式匹配函数
 * \param[in]  src 源字符串
 * \param[in]  pattern 模式字符串
 * \return     如果匹配成功，返回模式字符串后的位置指针；否则返回 NULL
 * \note       - 忽略 src 前导空格，匹配 pattern 全部内容
 *             - 该函数适用于长度小于等于 10Bytes 的模式匹配
 */
u8 *strmatch_s(u8 *src, u8 *pattern)
{
    while (*src == ' ') // 跳过前导空格
        src++;
    while (*pattern != '\0')
        if (*src++ != *pattern++) return NULL;
    return src;
}

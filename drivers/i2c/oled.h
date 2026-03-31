#ifndef __OLED_H
#define __OLED_H

#include "i2c.h"
#include "oled_data.h"

/* Exported Macros --------------------------------------------------------- */

// FontSize参数取值
// 此参数值不仅用于判断，而且用于计算横向字符偏移，默认值为字体像素宽度

#define OLED_8X16 8
#define OLED_6X8 6

// IsFilled参数数值

#define OLED_UNFILLED 0
#define OLED_FILLED 1

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void oled_init(void);

// 更新函数

void oled_update(void);
void oled_update_area(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

// 显存控制函数

void oled_clear(void);
void oled_clear_area(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void oled_reverse(void);
void oled_reverse_area(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

// 显示函数

void oled_show_char(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void oled_show_string(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void oled_show_num(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void oled_show_signed_num(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void oled_show_hex_num(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void oled_show_bin_num(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void oled_show_float_num(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void oled_show_image(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);
void oled_printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);
// void oled_ShowChinese(uint8_t X, uint8_t Y, char *Chinese);

// 绘图函数

uint8_t oled_get_point(int16_t X, int16_t Y);
void oled_draw_point(int16_t X, int16_t Y);
void oled_draw_line(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1);
void oled_draw_rectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled);
void oled_draw_triangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled);
void oled_draw_circle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled);
void oled_draw_ellipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled);
void oled_draw_arc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled);

#endif

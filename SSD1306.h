/*
 * SSD1306.h
 *
 *  Версия 1.0. Используется I2C
 *
 *  Created on: Apr 24, 2025
 *      Author: Oleg Volkov
 *
 *  YouTube: https://www.youtube.com/channel/UCzZKTNVpcMSALU57G1THoVw
 *  GitHub: https://github.com/Solderingironspb/Lessons-Stm32/blob/master/README.md
 *  Группа ВК: https://vk.com/solderingiron.stm32
 *
 *  Кодировка UTF-8 Basic Latin: https://www.utf8-chartable.de/unicode-utf8-table.pl
 *  Кодировка UTF-8 Cyrillic: https://www.utf8-chartable.de/unicode-utf8-table.pl?start=1024&names=-&unicodeinhtml=hex
 *  Программа для конвертации изображения.bmp в bitmap: http://en.radzio.dxp.pl/bitmap_converter/
 */

#ifndef INC_SSD1306_H_
#define INC_SSD1306_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define FONT_3x5             0  //Шрифт 3x5
#define FONT_5x7             1  //Шрифт 5x7
#define INVERSION_OFF        0  //Инверсия выключена
#define INVERSION_ON         1  //Инверсия включена
#define TIME_INTERVAL_SECOND 0  //Для графика. Интервал в секундах
#define TIME_INTERVAL_MINUTE 1  //Для графика. Интервал в минутах
#define TIME_INTERVAL_HOUR   2  //Для графика. Интервал в часах
#define GRID_OFF             0  //Для графика. Сетка выключена
#define GRID_ON              1  //Для графика. Сетка включена

/*========== Выберите используемый дисплей ==========*/
// #define SSD1306_64x32
// #define SSD1306_64x48
// #define SSD1306_72x40
// #define SSD1306_96x16
#define SSD1306_128x32
// #define SSD1306_128x64
// #define SH1106_128x64
/*========== Выберите используемый дисплей ==========*/

/*========== Укажите разрешение дисплея ==========*/
#define SSD1306_WIDTH  128  // Количество точек по горизонтали
#define SSD1306_HEIGHT 32   // Количество точек по вертикали
/*========== Укажите разрешение дисплея ==========*/

/*========== Укажите адрес дисплея на шине I2C и используемый I2C ==========*/
#define SSD1306_ADDR 0x3C  // Адрес SSD1306 на шине I2C
#define I2C_USE      I2C1  // I2C_TypeDef* I2C
/*========== Укажите адрес дисплея на шине I2C и используемый I2C ==========*/

#define SSD1306_COLOR_BLACK 0x00  // Закрасить пиксель черным цветом (Выключить пиксель)
#define SSD1306_COLOR_WHITE 0x01  // Закрасить пиксель белым цветом (Включить пиксель)

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_DEACTIVATE_SCROLL                    0x2E  // Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F  // Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3  // Set scroll range

#define SSD1306_SETLOWCOLUMN        0x00 /** Set Lower Column Start Address for Page Addressing Mode. */
#define SSD1306_SETHIGHCOLUMN       0x10 /** Set Higher Column Start Address for Page Addressing Mode. */
#define SSD1306_MEMORYMODE          0x20 /** Set Memory Addressing Mode. */
#define SSD1306_SETSTARTLINE        0x40 /** Set display RAM display start line register from 0 - 63. */
#define SSD1306_SETCONTRAST         0x81 /** Set Display Contrast to one of 256 steps. */
#define SSD1306_CHARGEPUMP          0x8D /** Enable or disable charge pump.  Follow with 0X14 enable, 0X10 disable. */
#define SSD1306_SEGREMAP            0xA0 /** Set Segment Re-map between data column and the segment driver. */
#define SSD1306_DISPLAYALLON_RESUME 0xA4 /** Resume display from GRAM content. */
#define SSD1306_DISPLAYALLON        0xA5 /** Force display on regardless of GRAM content. */
#define SSD1306_NORMALDISPLAY       0xA6 /** Set Normal Display. */
#define SSD1306_INVERTDISPLAY       0xA7 /** Set Inverse Display. */
#define SSD1306_SETMULTIPLEX        0xA8 /** Set Multiplex Ratio from 16 to 63. */
#define SSD1306_DISPLAYOFF          0xAE /** Set Display off. */
#define SSD1306_DISPLAYON           0xAF /** Set Display on. */
#define SSD1306_SETSTARTPAGE        0XB0 /**Set GDDRAM Page Start Address. */
#define SSD1306_COMSCANINC          0xC0 /** Set COM output scan direction normal. */
#define SSD1306_COMSCANDEC          0xC8 /** Set COM output scan direction reversed. */
#define SSD1306_SETDISPLAYOFFSET    0xD3 /** Set Display Offset. */
#define SSD1306_SETCOMPINS          0xDA /** Sets COM signals pin configuration to match the OLED panel layout. */
#define SSD1306_SETVCOMDETECT       0xDB /** This command adjusts the VCOMH regulator output. */
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 /** Set Display Clock Divide Ratio/ Oscillator Frequency. */
#define SSD1306_SETPRECHARGE        0xD9 /** Set Pre-charge Period */
#define SSD1306_DEACTIVATE_SCROLL   0x2E /** Deactivate scroll */
#define SSD1306_NOP                 0XE3 /** No Operation Command. */

#define SH1106_SET_PUMP_VOLTAGE 0X30 /** Set Pump voltage value: (30H~33H) 6.4, 7.4, 8.0 (POR), 9.0. */
#define SH1106_SET_PUMP_MODE    0XAD /** First byte of set charge pump mode */
#define SH1106_PUMP_ON          0X8B /** Second byte charge pump on. */
#define SH1106_PUMP_OFF         0X8A /** Second byte charge pump off. */

typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

void SSD1306_logo_demonstration(void);

void SSD1306_WriteCommand(uint8_t data);
void SSD1306_WriteData(uint8_t *data, uint16_t count);
uint8_t SSD1306_Init(void);
void SSD1306_Update(void);
void SSD1306_Clean_Frame_buffer(void);
void SSD1306_Fill(uint8_t color);
void SSD1306_Draw_pixel(uint16_t x, uint16_t y, uint8_t color);
void SSD1306_Contrast(uint8_t value);
void SSD1306_Decode_UTF8(uint8_t x, uint8_t y, uint8_t font, bool inversion, char *tx_buffer);
void SSD1306_Print_symbol_3x5(uint8_t x, uint8_t y, uint16_t symbol, uint8_t inversion);
void SSD1306_Print_symbol_5x7(uint8_t x, uint8_t y, uint16_t symbol, uint8_t inversion);
void SSD1306_DrawBitmap(const uint8_t *bitmap, int8_t x, int8_t y, int8_t w, int8_t h);
void SSD1306_Inversion(int i);
uint8_t SSD1306_Value_for_Plot(int y_min, int y_max, float value);
void SSD1306_Fill_the_array_Plot(uint8_t *counter, uint8_t *array, uint8_t size_array, bool *array_is_full, uint8_t value);
void SSD1306_Generate_a_Graph(uint8_t *counter, uint8_t *array, uint8_t size_array, bool *array_is_full, int y_min, int y_max, uint8_t x_grid_time, uint8_t time_interval, bool grid);
void SSD1306_Draw_line(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color);
void SSD1306_Draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
void SSD1306_Draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
void SSD1306_Draw_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color);
void SSD1306_Draw_circle_filled(int16_t x, int16_t y, int16_t radius, uint8_t color);
void SSD1306_Draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color);
void SSD1306_Draw_triangle_filled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color);

#endif /* INC_SSD1306_H_ */
/*
 * SSD1306.c
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

#include "SSD1306.h"
#include "stm32f103xx_CMSIS.h"

// clang-format off
char tx_buffer[128] = { 0, };  //Буфер для отправки текста на дисплей
uint8_t SSD1306_Frame_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = { 0, }; //Буфер кадра

// Для работы отрисовки графика:
uint8_t cnt = 0;                 // счетчик накопления значений в окне графика
const uint8_t size_array = 100;  // размер массива. В нашем случае ширина 100 точек(График 100*50 пикселей)
uint8_t arr[100] = { 0,};        // значения на графике. Заполняются в определенный момент времени(каждый шаг сдвига графика влево)
bool array_is_full = false;      // значения заполнили массив, можно сдвигать график влево
// clang-format on

static SSD1306_t SSD1306;

/*-----------------------------------Шрифт 3*5----------------------------------*/
const uint8_t Font_3x5[] = {
    0x00, 0x00, 0x00,  // 0/ --> space     20
    0x17, 0x00, 0x00,  // 1/ --> !         21
    0x03, 0x00, 0x03,  // 2/ --> "         22 и т.д.
    0xFF, 0x0A, 0xFF,  // 3/ --> #
    0x17, 0xFF, 0x1D,  // 4/ --> $
    0x09, 0x04, 0x12,  // 5/ --> %
    0x0E, 0x1B, 0x0A,  // 6/ --> &
    0x03, 0x00, 0x00,  // 7/ --> '
    0x0E, 0x11, 0x00,  // 8/ --> (
    0x11, 0x0E, 0x00,  // 9/ --> )
    0x04, 0x0A, 0x04,  // 10/ --> *
    0x04, 0x0E, 0x04,  // 11/ --> +
    0x10, 0x18, 0x00,  // 12/ --> ,
    0x04, 0x04, 0x04,  // 13/ --> -
    0x10, 0x00, 0x00,  // 14/ --> .
    0x08, 0x04, 0x02,  // 15/ --> /
    0xFF, 0x11, 0xFF,  // 16/ --> 0
    0x11, 0xFF, 0x10,  // 17/ --> 1
    0x1D, 0x15, 0x17,  // 18/ --> 2
    0x15, 0x15, 0xFF,  // 19/ --> 3
    0x07, 0x04, 0xFF,  // 20/ --> 4
    0x17, 0x15, 0x1D,  // 21/ --> 5
    0xFF, 0x15, 0x1D,  // 22/ --> 6
    0x01, 0x01, 0xFF,  // 23/ --> 7
    0xFF, 0x15, 0xFF,  // 24/ --> 8
    0x17, 0x15, 0x1F,  // 25/ --> 9
    0x0A, 0x00, 0x00,  // 26/ --> :
    0x10, 0x0A, 0x00,  // 27/ --> ;
    0x04, 0x0A, 0x00,  // 28/ --> <
    0x0A, 0x0A, 0x0A,  // 29/ --> =
    0x0A, 0x04, 0x00,  // 30/ --> >
    0x01, 0x15, 0x07,  // 31/ --> ?
    0x1F, 0x11, 0x17,  // 32/ --> @
    0x1F, 0x05, 0x1F,  // 33/ --> A
    0x1F, 0x15, 0x1B,  // 34/ --> B
    0x1F, 0x11, 0x11,  // 35/ --> C
    0x1F, 0x11, 0x0E,  // 36/ --> D
    0x1F, 0x15, 0x15,  // 37/ --> E
    0x1F, 0x05, 0x05,  // 38/ --> F
    0x1F, 0x11, 0x1D,  // 39/ --> G
    0x1F, 0x04, 0x1F,  // 40/ --> H
    0x11, 0x1F, 0x11,  // 41/ --> I
    0x18, 0x10, 0x1F,  // 42/ --> J
    0x1F, 0x04, 0x1B,  // 43/ --> K
    0x1F, 0x10, 0x10,  // 44/ --> L
    0x1F, 0x02, 0x1F,  // 45/ --> M
    0x1F, 0x01, 0x1F,  // 46/ --> N
    0x1F, 0x11, 0x1F,  // 47/ --> O
    0x1F, 0x05, 0x07,  // 48/ --> P
    0x0F, 0x19, 0x0F,  // 49/ --> Q
    0x1F, 0x05, 0x1B,  // 50/ --> R
    0x17, 0x15, 0x1D,  // 51/ --> S
    0x01, 0x1F, 0x01,  // 52/ --> T
    0x1F, 0x10, 0x1F,  // 53/ --> U
    0x0F, 0x10, 0x0F,  // 54/ --> V
    0x1F, 0x08, 0x1F,  // 55/ --> W
    0x1B, 0x04, 0x1B,  // 56/ --> X
    0x03, 0x1C, 0x03,  // 57/ --> Y
    0x19, 0x15, 0x13,  // 58/ --> Z
    0x1F, 0x11, 0x00,  // 59/ --> [
    0x02, 0x04, 0x08,  // 60/ --> '\'
    0x11, 0x1F, 0x00,  // 61/ --> ]
    0x02, 0x01, 0x02,  // 62/ --> ^
    0x10, 0x10, 0x10,  // 63/ --> _
    0x01, 0x02, 0x00,  // 64/ --> `
    0x0C, 0x12, 0x1E,  // 65/ --> a
    0x1E, 0x14, 0x08,  // 66/ --> b
    0x1C, 0x14, 0x14,  // 67/ --> c
    0x0C, 0x12, 0x1F,  // 68/ --> d
    0x0C, 0x1A, 0x14,  // 69/ --> e
    0x04, 0x1F, 0x05,  // 70/ --> f
    0x17, 0x15, 0x0F,  // 71/ --> g
    0x1F, 0x02, 0x1C,  // 72/ --> h
    0x00, 0x1D, 0x00,  // 73/ --> i
    0x10, 0x0D, 0x00,  // 74/ --> j
    0x1F, 0x04, 0x1A,  // 75/ --> k
    0x01, 0x1F, 0x00,  // 76/ --> l
    0x1E, 0x04, 0x1E,  // 77/ --> m
    0x1E, 0x02, 0x1E,  // 78/ --> n
    0x1E, 0x12, 0x1E,  // 79/ --> o
    0x1E, 0x0A, 0x04,  // 80/ --> p
    0x04, 0x0A, 0x1E,  // 81/ --> q
    0x1E, 0x02, 0x06,  // 82/ --> r
    0x14, 0x12, 0x0A,  // 83/ --> s
    0x02, 0x1F, 0x12,  // 84/ --> t
    0x1E, 0x10, 0x1E,  // 85/ --> u
    0x0E, 0x10, 0x0E,  // 86/ --> v
    0x1E, 0x08, 0x1E,  // 87/ --> w
    0x1A, 0x04, 0x1A,  // 88/ --> x
    0x13, 0x14, 0x0F,  // 89/ --> y
    0x1A, 0x12, 0x16,  // 90/ --> z
    0x04, 0x1F, 0x11,  // 91/ --> {
    0x00, 0x1F, 0x00,  // 92/ --> |
    0x11, 0x1F, 0x04,  // 93/ --> }
    0x0C, 0x04, 0x06,  // 94/ --> ~
    0x1F, 0x05, 0x1F,  // 95/ --> А
    0x1F, 0x15, 0x1D,  // 96/ --> Б
    0x1F, 0x15, 0x1B,  // 97/ --> В
    0x1F, 0x01, 0x01,  // 98/ --> Г
    0x1E, 0x11, 0x1E,  // 99/ --> Д
    0x1F, 0x15, 0x15,  // 100/ --> Е
    0x1B, 0x1F, 0x1B,  // 101/ --> Ж
    0x15, 0x15, 0x1B,  // 102/ --> З
    0x1F, 0x10, 0x1F,  // 103/ --> И
    0x1D, 0x11, 0x1D,  // 104/ --> Й
    0x1F, 0x04, 0x1B,  // 105/ --> К
    0x1E, 0x01, 0x1F,  // 106/ --> Л
    0x1F, 0x02, 0x1F,  // 107/ --> М
    0x1F, 0x04, 0x1F,  // 108/ --> Н
    0x1F, 0x11, 0x1F,  // 109/ --> О
    0x1F, 0x01, 0x1F,  // 110/ --> П
    0x1F, 0x05, 0x07,  // 111/ --> Р
    0x1F, 0x11, 0x11,  // 112/ --> С
    0x01, 0x1F, 0x01,  // 113/ --> Т
    0x13, 0x14, 0x1F,  // 114/ --> У
    0x0E, 0x1B, 0x0E,  // 115/ --> Ф
    0x1B, 0x04, 0x1B,  // 116/ --> Х
    0x0F, 0x08, 0x1F,  // 117/ --> Ц
    0x07, 0x04, 0x1F,  // 118/ --> Ч
    0x1F, 0x18, 0x1F,  // 119/ --> Ш
    0x0F, 0x0C, 0x1F,  // 120/ --> Щ
    0x01, 0x1F, 0x1C,  // 121/ --> Ъ
    0x1F, 0x14, 0x1F,  // 122/ --> Ы
    0x15, 0x15, 0x0E,  // 123/ --> Э
    0x1F, 0x14, 0x08,  // 124/ --> Ь
    0x1F, 0x0E, 0x1F,  // 125/ --> Ю
    0x1B, 0x05, 0x1F,  // 126/ --> Я
    0x0C, 0x12, 0x1E,  // 127/ --> a
    0x1E, 0x15, 0x1D,  // 128/ --> б
    0x1E, 0x16, 0x0C,  // 129/ --> в
    0x1E, 0x02, 0x02,  // 130/ --> г
    0x1C, 0x12, 0x1C,  // 131/ --> д
    0x0C, 0x1A, 0x14,  // 132/ --> е
    0x1A, 0x1E, 0x1A,  // 133/ --> ж
    0x12, 0x16, 0x1E,  // 134/ --> з
    0x1E, 0x10, 0x1E,  // 135/ --> и
    0x1A, 0x12, 0x1A,  // 136/ --> й
    0x1E, 0x04, 0x1A,  // 137/ --> к
    0x1C, 0x02, 0x1E,  // 138/ --> л
    0x1E, 0x04, 0x1E,  // 139/ --> м
    0x1E, 0x08, 0x1E,  // 140/ --> н
    0x1E, 0x12, 0x1E,  // 141/ --> о
    0x1E, 0x02, 0x1E,  // 142/ --> п
    0x1E, 0x0A, 0x04,  // 143/ --> р
    0x1E, 0x12, 0x12,  // 144/ --> с
    0x02, 0x1E, 0x02,  // 145/ --> т
    0x16, 0x14, 0x0E,  // 146/ --> у
    0x0C, 0x1E, 0x0C,  // 147/ --> ф
    0x1A, 0x0C, 0x1A,  // 148/ --> х
    0x0E, 0x08, 0x1E,  // 149/ --> ц
    0x06, 0x04, 0x1E,  // 150/ --> ч
    0x1E, 0x18, 0x1E,  // 151/ --> ш
    0x0E, 0x0C, 0x1E,  // 152/ --> щ
    0x02, 0x1E, 0x18,  // 153/ --> ъ
    0x1E, 0x14, 0x1E,  // 154/ --> ы
    0x1E, 0x18, 0x00,  // 155/ --> ь
    0x12, 0x16, 0x0C,  // 156/ --> э
    0x1E, 0x0C, 0x1E,  // 157/ --> ю
    0x14, 0x0A, 0x1E,  // 158/ --> я
    0x1F, 0x15, 0x15,  // 159/ --> Ё
    0x0C, 0x1A, 0x14,  // 160/ --> ё
    0x03, 0x03, 0x00,  // 161/ --> °
};
/*-----------------------------------Шрифт 3*5----------------------------------*/

/*-----------------------------------Шрифт 5*7----------------------------------*/
const uint8_t Font_5x7[] = {
    0x00, 0x00, 0x00, 0x00, 0x00,  // 0/ --> space     20
    0x00, 0x4F, 0x00, 0x00, 0x00,  // 1/ --> !         21
    0x07, 0x00, 0x07, 0x00, 0x00,  // 2/ --> "         22 и т.д.
    0x14, 0x7F, 0x14, 0x7F, 0x14,  // 3/ --> #
    0x24, 0x2A, 0x7F, 0x2A, 0x12,  // 4/ --> $
    0x23, 0x13, 0x08, 0x64, 0x62,  // 5/ --> %
    0x36, 0x49, 0x55, 0x22, 0x40,  // 6/ --> &
    0x00, 0x05, 0x03, 0x00, 0x00,  // 7/ --> '
    0x1C, 0x22, 0x41, 0x00, 0x00,  // 8/ --> (
    0x41, 0x22, 0x1C, 0x00, 0x00,  // 9/ --> )
    0x14, 0x08, 0x3E, 0x08, 0x14,  // 10/ --> *
    0x08, 0x08, 0x3E, 0x08, 0x08,  // 11/ --> +
    0xA0, 0x60, 0x00, 0x00, 0x00,  // 12/ --> ,
    0x08, 0x08, 0x08, 0x08, 0x08,  // 13/ --> -
    0x60, 0x60, 0x00, 0x00, 0x00,  // 14/ --> .
    0x20, 0x10, 0x08, 0x04, 0x02,  // 15/ --> /
    0x3E, 0x51, 0x49, 0x45, 0x3E,  // 16/ --> 0
    0x00, 0x42, 0x7F, 0x40, 0x00,  // 17/ --> 1
    0x42, 0x61, 0x51, 0x49, 0x46,  // 18/ --> 2
    0x21, 0x41, 0x45, 0x4B, 0x31,  // 19/ --> 3
    0x18, 0x14, 0x12, 0x7F, 0x10,  // 20/ --> 4
    0x27, 0x45, 0x45, 0x45, 0x39,  // 21/ --> 5
    0x3C, 0x4A, 0x49, 0x49, 0x30,  // 22/ --> 6
    0x01, 0x71, 0x09, 0x05, 0x03,  // 23/ --> 7
    0x36, 0x49, 0x49, 0x49, 0x36,  // 24/ --> 8
    0x06, 0x49, 0x49, 0x29, 0x1E,  // 25/ --> 9
    0x6C, 0x6C, 0x00, 0x00, 0x00,  // 26/ --> :
    0x00, 0x56, 0x36, 0x00, 0x00,  // 27/ --> ;
    0x08, 0x14, 0x22, 0x41, 0x00,  // 28/ --> <
    0x24, 0x24, 0x24, 0x24, 0x24,  // 29/ --> =
    0x00, 0x41, 0x22, 0x14, 0x08,  // 30/ --> >
    0x02, 0x01, 0x51, 0x09, 0x06,  // 31/ --> ?
    0x32, 0x49, 0x79, 0x41, 0x3E,  // 32/ --> @
    0x7E, 0x11, 0x11, 0x11, 0x7E,  // 33/ --> A
    0x7F, 0x49, 0x49, 0x49, 0x36,  // 34/ --> B
    0x3E, 0x41, 0x41, 0x41, 0x22,  // 35/ --> C
    0x7F, 0x41, 0x41, 0x22, 0x1C,  // 36/ --> D
    0x7F, 0x49, 0x49, 0x49, 0x41,  // 37/ --> E
    0x7F, 0x09, 0x09, 0x09, 0x01,  // 38/ --> F
    0x3E, 0x41, 0x49, 0x49, 0x3A,  // 39/ --> G
    0x7F, 0x08, 0x08, 0x08, 0x7F,  // 40/ --> H
    0x00, 0x41, 0x7F, 0x41, 0x00,  // 41/ --> I
    0x20, 0x40, 0x41, 0x3F, 0x01,  // 42/ --> J
    0x7F, 0x08, 0x14, 0x22, 0x41,  // 43/ --> K
    0x7F, 0x40, 0x40, 0x40, 0x40,  // 44/ --> L
    0x7F, 0x02, 0x0C, 0x02, 0x7F,  // 45/ --> M
    0x7F, 0x04, 0x08, 0x10, 0x7F,  // 46/ --> N
    0x3E, 0x41, 0x41, 0x41, 0x3E,  // 47/ --> O
    0x7F, 0x09, 0x09, 0x09, 0x06,  // 48/ --> P
    0x3E, 0x41, 0x51, 0x21, 0x5E,  // 49/ --> Q
    0x7F, 0x09, 0x19, 0x29, 0x46,  // 50/ --> R
    0x46, 0x49, 0x49, 0x49, 0x31,  // 51/ --> S
    0x01, 0x01, 0x7F, 0x01, 0x01,  // 52/ --> T
    0x3F, 0x40, 0x40, 0x40, 0x3F,  // 53/ --> U
    0x1F, 0x20, 0x40, 0x20, 0x1F,  // 54/ --> V
    0x3F, 0x40, 0x60, 0x40, 0x3F,  // 55/ --> W
    0x63, 0x14, 0x08, 0x14, 0x63,  // 56/ --> X
    0x07, 0x08, 0x70, 0x08, 0x07,  // 57/ --> Y
    0x61, 0x51, 0x49, 0x45, 0x43,  // 58/ --> Z
    0x7F, 0x41, 0x41, 0x00, 0x00,  // 59/ --> [
    0x15, 0x16, 0x7C, 0x16, 0x15,  // 60/ --> '\'
    0x41, 0x41, 0x7F, 0x00, 0x00,  // 61/ --> ]
    0x04, 0x02, 0x01, 0x02, 0x04,  // 62/ --> ^
    0x40, 0x40, 0x40, 0x40, 0x40,  // 63/ --> _
    0x01, 0x02, 0x04, 0x00, 0x00,  // 64/ --> `
    0x20, 0x54, 0x54, 0x54, 0x78,  // 65/ --> a
    0x7F, 0x44, 0x44, 0x44, 0x38,  // 66/ --> b
    0x38, 0x44, 0x44, 0x44, 0x00,  // 67/ --> c
    0x38, 0x44, 0x44, 0x48, 0x7F,  // 68/ --> d
    0x38, 0x54, 0x54, 0x54, 0x18,  // 69/ --> e
    0x10, 0x7E, 0x11, 0x01, 0x02,  // 70/ --> f
    0x0C, 0x52, 0x52, 0x52, 0x3E,  // 71/ --> g
    0x7F, 0x08, 0x04, 0x04, 0x78,  // 72/ --> h
    0x00, 0x44, 0x7D, 0x40, 0x00,  // 73/ --> i
    0x20, 0x40, 0x40, 0x3D, 0x00,  // 74/ --> j
    0x7F, 0x10, 0x28, 0x44, 0x00,  // 75/ --> k
    0x00, 0x41, 0x7F, 0x40, 0x00,  // 76/ --> l
    0x7C, 0x04, 0x18, 0x04, 0x78,  // 77/ --> m
    0x7C, 0x08, 0x04, 0x04, 0x78,  // 78/ --> n
    0x38, 0x44, 0x44, 0x44, 0x38,  // 79/ --> o
    0x7C, 0x14, 0x14, 0x14, 0x08,  // 80/ --> p
    0x08, 0x14, 0x14, 0x18, 0x7C,  // 81/ --> q
    0x7C, 0x08, 0x04, 0x04, 0x08,  // 82/ --> r
    0x48, 0x54, 0x54, 0x54, 0x20,  // 83/ --> s
    0x04, 0x3F, 0x44, 0x40, 0x20,  // 84/ --> t
    0x3C, 0x40, 0x40, 0x20, 0x7C,  // 85/ --> u
    0x1C, 0x20, 0x40, 0x20, 0x1C,  // 86/ --> v
    0x1E, 0x20, 0x10, 0x20, 0x1E,  // 87/ --> w
    0x22, 0x14, 0x08, 0x14, 0x22,  // 88/ --> x
    0x06, 0x48, 0x48, 0x48, 0x3E,  // 89/ --> y
    0x44, 0x64, 0x54, 0x4C, 0x44,  // 90/ --> z
    0x08, 0x36, 0x41, 0x00, 0x00,  // 91/ --> {
    0x00, 0x00, 0x7F, 0x00, 0x00,  // 92/ --> |
    0x41, 0x36, 0x08, 0x00, 0x00,  // 93/ --> }
    0x08, 0x08, 0x2A, 0x1C, 0x08,  // 94/ --> ~
    0x7E, 0x11, 0x11, 0x11, 0x7E,  // 95/ --> А
    0x7F, 0x49, 0x49, 0x49, 0x33,  // 96/ --> Б
    0x7F, 0x49, 0x49, 0x49, 0x36,  // 97/ --> В
    0x7F, 0x01, 0x01, 0x01, 0x03,  // 98/ --> Г
    0xE0, 0x51, 0x4F, 0x41, 0xFF,  // 99/ --> Д
    0x7F, 0x49, 0x49, 0x49, 0x41,  // 100/ --> Е
    0x77, 0x08, 0x7F, 0x08, 0x77,  // 101/ --> Ж
    0x41, 0x49, 0x49, 0x49, 0x36,  // 102/ --> З
    0x7F, 0x10, 0x08, 0x04, 0x7F,  // 103/ --> И
    0x7C, 0x21, 0x12, 0x09, 0x7C,  // 104/ --> Й
    0x7F, 0x08, 0x14, 0x22, 0x41,  // 105/ --> К
    0x20, 0x41, 0x3F, 0x01, 0x7F,  // 106/ --> Л
    0x7F, 0x02, 0x0C, 0x02, 0x7F,  // 107/ --> М
    0x7F, 0x08, 0x08, 0x08, 0x7F,  // 108/ --> Н
    0x3E, 0x41, 0x41, 0x41, 0x3E,  // 109/ --> О
    0x7F, 0x01, 0x01, 0x01, 0x7F,  // 110/ --> П
    0x7F, 0x09, 0x09, 0x09, 0x06,  // 111/ --> Р
    0x3E, 0x41, 0x41, 0x41, 0x22,  // 112/ --> С
    0x01, 0x01, 0x7F, 0x01, 0x01,  // 113/ --> Т
    0x47, 0x28, 0x10, 0x08, 0x07,  // 114/ --> У
    0x1C, 0x22, 0x7F, 0x22, 0x1C,  // 115/ --> Ф
    0x63, 0x14, 0x08, 0x14, 0x63,  // 116/ --> Х
    0x7F, 0x40, 0x40, 0x40, 0xFF,  // 117/ --> Ц
    0x07, 0x08, 0x08, 0x08, 0x7F,  // 118/ --> Ч
    0x7F, 0x40, 0x7F, 0x40, 0x7F,  // 119/ --> Ш
    0x7F, 0x40, 0x7F, 0x40, 0xFF,  // 120/ --> Щ
    0x01, 0x7F, 0x48, 0x48, 0x30,  // 121/ --> Ъ
    0x7F, 0x48, 0x30, 0x00, 0x7F,  // 122/ --> Ы
    0x00, 0x7F, 0x48, 0x48, 0x30,  // 123/ --> Э
    0x22, 0x41, 0x49, 0x49, 0x3E,  // 124/ --> Ь
    0x7F, 0x08, 0x3E, 0x41, 0x3E,  // 125/ --> Ю
    0x46, 0x29, 0x19, 0x09, 0x7f,  // 126/ --> Я
    0x20, 0x54, 0x54, 0x54, 0x78,  // 127/ --> a
    0x3c, 0x4a, 0x4a, 0x49, 0x31,  // 128/ --> б
    0x7c, 0x54, 0x54, 0x28, 0x00,  // 129/ --> в
    0x7c, 0x04, 0x04, 0x04, 0x0c,  // 130/ --> г
    0xe0, 0x54, 0x4c, 0x44, 0xfc,  // 131/ --> д
    0x38, 0x54, 0x54, 0x54, 0x18,  // 132/ --> е
    0x6c, 0x10, 0x7c, 0x10, 0x6c,  // 133/ --> ж
    0x44, 0x44, 0x54, 0x54, 0x28,  // 134/ --> з
    0x7c, 0x20, 0x10, 0x08, 0x7c,  // 135/ --> и
    0x7c, 0x41, 0x22, 0x11, 0x7c,  // 136/ --> й
    0x7c, 0x10, 0x28, 0x44, 0x00,  // 137/ --> к
    0x20, 0x44, 0x3c, 0x04, 0x7c,  // 138/ --> л
    0x7c, 0x08, 0x10, 0x08, 0x7c,  // 139/ --> м
    0x7c, 0x10, 0x10, 0x10, 0x7c,  // 140/ --> н
    0x38, 0x44, 0x44, 0x44, 0x38,  // 141/ --> о
    0x7c, 0x04, 0x04, 0x04, 0x7c,  // 142/ --> п
    0x7C, 0x14, 0x14, 0x14, 0x08,  // 143/ --> р
    0x38, 0x44, 0x44, 0x44, 0x28,  // 144/ --> с
    0x04, 0x04, 0x7c, 0x04, 0x04,  // 145/ --> т
    0x0C, 0x50, 0x50, 0x50, 0x3C,  // 146/ --> у
    0x30, 0x48, 0xfc, 0x48, 0x30,  // 147/ --> ф
    0x44, 0x28, 0x10, 0x28, 0x44,  // 148/ --> х
    0x7c, 0x40, 0x40, 0x40, 0xfc,  // 149/ --> ц
    0x0c, 0x10, 0x10, 0x10, 0x7c,  // 150/ --> ч
    0x7c, 0x40, 0x7c, 0x40, 0x7c,  // 151/ --> ш
    0x7c, 0x40, 0x7c, 0x40, 0xfc,  // 152/ --> щ
    0x04, 0x7c, 0x50, 0x50, 0x20,  // 153/ --> ъ
    0x7c, 0x50, 0x50, 0x20, 0x7c,  // 154/ --> ы
    0x7c, 0x50, 0x50, 0x20, 0x00,  // 155/ --> э
    0x28, 0x44, 0x54, 0x54, 0x38,  // 156/ --> ь
    0x7c, 0x10, 0x38, 0x44, 0x38,  // 157/ --> ю
    0x08, 0x54, 0x34, 0x14, 0x7c,  // 158/ --> я
    0x7E, 0x4B, 0x4A, 0x4B, 0x42,  // 159/ --> Ё
    0x38, 0x55, 0x54, 0x55, 0x18,  // 160/ --> ё
    0x00, 0x06, 0x09, 0x09, 0x06,  // 161/ --> °
};

/*-----------------------------------Шрифт 5*7----------------------------------*/

/*================= Демонстрационное лого. Можно вырезать. =====================*/

// clang-format off
const char solderingiron[] = {
    0xC0, 0xE0, 0x30, 0x30, 0x30, 0x30, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
    0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0xC1, 0x83, 0x06, 0x04, 0x0C, 0x0C, 0x8C, 0xF8, 0x70, 0x00, 0x78, 0xFC, 0x86, 0x03, 0x03, 0x03, 0x03, 0x86, 0xFC, 
    0x78, 0x00, 0xFF, 0xFF, 0x78, 0xFE, 0x86, 0x03, 0x03, 0x03, 0x86, 0xFF, 0xFF, 0x00, 0x00, 0xFC, 0xFE, 0xB7, 0x33, 0x33, 0x33, 
    0x37, 0x3E, 0x3C, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x03, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x03, 0x03, 0x06, 0xFE, 
    0xF8, 0x00, 0x00, 0x38, 0xFC, 0x86, 0x03, 0x03, 0x03, 0x86, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x03, 0x78, 0xFC, 0x86, 0x03, 0x03, 0x03, 0x03, 0x86, 0xFC, 0x78, 0x00, 
    0x00, 0xFF, 0xFF, 0x06, 0x03, 0x03, 0x03, 0x06, 0xFE, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x08, 0x09, 0x09, 0x0B, 0x0B, 0x0B, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 0x09, 0x0B, 0x0B, 0x0B, 0x0B, 
    0x09, 0x08, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x09, 0x09, 0x0B, 0x0B, 0x0B, 0x09, 0x0B, 0x0B, 0x08, 0x08, 0x08, 0x09, 0x0B, 0x0B, 
    0x0B, 0x0B, 0x0B, 0x09, 0x09, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x0B, 0x0B, 0x08, 0x08, 0x08, 0x08, 
    0x08, 0x0B, 0x0B, 0x08, 0x08, 0x00, 0x10, 0x39, 0x63, 0x63, 0x63, 0x31, 0x1F, 0x0F, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 
    0x08, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x0B, 0x0B, 0x0B, 0x0B, 0x09, 0x08, 
    0x08, 0x08, 0x08, 0x0B, 0x0B, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0B, 0x0B
};
// clang-format on

/**
 ******************************************************************************
 *  @breif Функция вывода стартового демонстрационного лого
 ******************************************************************************
 */
void SSD1306_logo_demonstration(void) {
    SSD1306_Clean_Frame_buffer();
    for (int i = 0; i < 369; i++) {
        SSD1306_Frame_Buffer[i + 265] = solderingiron[i];
    }
    SSD1306_Update();
    sprintf(tx_buffer, "Saint Petersburg");
    SSD1306_Decode_UTF8(16, 40, 1, 0, tx_buffer);
    sprintf(tx_buffer, "electronics lab.");
    SSD1306_Decode_UTF8(18, 48, 1, 0, tx_buffer);
    Delay_ms(500);
    SSD1306_Update();
    Delay_ms(3000);
    sprintf(tx_buffer, "                ");
    SSD1306_Decode_UTF8(18, 48, 1, 0, tx_buffer);
    Delay_ms(500);
    SSD1306_Update();
    sprintf(tx_buffer, "                ");
    SSD1306_Decode_UTF8(16, 40, 1, 0, tx_buffer);
    Delay_ms(500);
    SSD1306_Update();

    for (int i = 0; i < 369; i++) {
        SSD1306_Frame_Buffer[i + 265] = 0x00;
    }
    SSD1306_Update();
    Delay_ms(500);
    SSD1306_Clean_Frame_buffer();
    SSD1306_Update();
}

/**
 ******************************************************************************
 *  @breif Функция отправки команды на дисплей SSD1306
 *  @param  data - команда
 ******************************************************************************
 */

void SSD1306_Send_command(uint8_t data) {
    uint8_t dt[2];
    dt[0] = 0x00;
    dt[1] = data;
    CMSIS_I2C_Data_Transmit(I2C_USE, SSD1306_ADDR, dt, 2, 100);
}

/**
 ******************************************************************************
 *  @breif Функция отправки данных на дисплей SSD1306
 *  @param  data - данные
 ******************************************************************************
 */

void SSD1306_Send_data(uint8_t *data, uint16_t count) {
    uint8_t dt[256];
    dt[0] = 0x40;
    uint8_t i;
    for (i = 0; i < count; i++) {
        dt[i + 1] = data[i];
    }
    CMSIS_I2C_Data_Transmit(I2C_USE, SSD1306_ADDR, dt, count + 1, 100);
}

/**
 ******************************************************************************
 *  @breif Функция инициализации дисплея
 ******************************************************************************
 */

uint8_t SSD1306_Init(void) {
    Delay_ms(100);

#ifdef SSD1306_64x32
    SSD1306_Send_command(SSD1306_DISPLAYOFF);
    SSD1306_Send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_Send_command(0x80);  // the suggested ratio 0x80
    SSD1306_Send_command(SSD1306_SETMULTIPLEX);
    SSD1306_Send_command(0x1F);  // ratio 32 (устанавливаем размер в пикселях по высоте -1 в HEX например 0x27 = 39 ( 40-1 ) )
    SSD1306_Send_command(0xAD);
    SSD1306_Send_command(0x30);
    SSD1306_Send_command(SSD1306_SETDISPLAYOFFSET);
    SSD1306_Send_command(0x0);                          // no offset
    SSD1306_Send_command(SSD1306_SETSTARTLINE | 0x00);  // line #0
    SSD1306_Send_command(SSD1306_CHARGEPUMP);
    SSD1306_Send_command(0x14);  // internal vcc
    SSD1306_Send_command(SSD1306_NORMALDISPLAY);
    SSD1306_Send_command(SSD1306_DISPLAYALLON_RESUME);
    SSD1306_Send_command(SSD1306_SEGREMAP | 0x01);  // column 127 mapped to SEG0
    SSD1306_Send_command(SSD1306_COMSCANDEC);       // column scan direction reversed
    SSD1306_Send_command(SSD1306_SETCOMPINS);
    SSD1306_Send_command(0x02);  // 0x12 if height > 32 else 0x02 ( если вісота больше 32 ставим 0х12 , если равно 32 или меньше то 0х02 )
    SSD1306_Send_command(SSD1306_SETCONTRAST);
    SSD1306_Send_command(0x7F);  // contrast level 127
    SSD1306_Send_command(SSD1306_SETPRECHARGE);
    SSD1306_Send_command(0xF1);  // pre-charge period (1, 15)
    SSD1306_Send_command(SSD1306_SETVCOMDETECT);
    SSD1306_Send_command(0x40);  // vcomh regulator level
    SSD1306_Send_command(SSD1306_DISPLAYON);

#elif defined SSD1306_64x48
    SSD1306_Send_command(SSD1306_DISPLAYOFF);
    SSD1306_Send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_Send_command(0x80);  // the suggested ratio 0x80
    SSD1306_Send_command(SSD1306_SETMULTIPLEX);
    SSD1306_Send_command(0x2F);  // ratio 32 (устанавливаем размер в пикселях по высоте -1 в HEX например 0x27 = 39 ( 40-1 ) )
    SSD1306_Send_command(0xAD);
    SSD1306_Send_command(0x30);
    SSD1306_Send_command(SSD1306_SETDISPLAYOFFSET);
    SSD1306_Send_command(0x0);                          // no offset
    SSD1306_Send_command(SSD1306_SETSTARTLINE | 0x00);  // line #0
    SSD1306_Send_command(SSD1306_CHARGEPUMP);
    SSD1306_Send_command(0x14);  // internal vcc
    SSD1306_Send_command(SSD1306_NORMALDISPLAY);
    SSD1306_Send_command(SSD1306_DISPLAYALLON_RESUME);
    SSD1306_Send_command(SSD1306_SEGREMAP | 0x01);  // column 127 mapped to SEG0
    SSD1306_Send_command(SSD1306_COMSCANDEC);       // column scan direction reversed
    SSD1306_Send_command(SSD1306_SETCOMPINS);
    SSD1306_Send_command(0x12);  // 0x12 if height > 32 else 0x02 ( если вісота больше 32 ставим 0х12 , если равно 32 или меньше то 0х02 )
    SSD1306_Send_command(SSD1306_SETCONTRAST);
    SSD1306_Send_command(0x7F);  // contrast level 127
    SSD1306_Send_command(SSD1306_SETPRECHARGE);
    SSD1306_Send_command(0xF1);  // pre-charge period (1, 15)
    SSD1306_Send_command(SSD1306_SETVCOMDETECT);
    SSD1306_Send_command(0x40);  // vcomh regulator level
    SSD1306_Send_command(SSD1306_DISPLAYON);

#elif defined SSD1306_72x40
    SSD1306_Send_command(SSD1306_DISPLAYOFF);
    SSD1306_Send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_Send_command(0x80);  // the suggested ratio 0x80
    SSD1306_Send_command(SSD1306_SETMULTIPLEX);
    SSD1306_Send_command(0x27);  // ratio 32 (устанавливаем размер в пикселях по высоте -1 в HEX например 0x27 = 39 ( 40-1 ) )
    SSD1306_Send_command(0xAD);
    SSD1306_Send_command(0x30);
    SSD1306_Send_command(SSD1306_SETDISPLAYOFFSET);
    SSD1306_Send_command(0x0);                          // no offset
    SSD1306_Send_command(SSD1306_SETSTARTLINE | 0x00);  // line #0
    SSD1306_Send_command(SSD1306_CHARGEPUMP);
    SSD1306_Send_command(0x14);  // internal vcc
    SSD1306_Send_command(SSD1306_MEMORYMODE);
    SSD1306_Send_command(0x02);                     // page mode
    SSD1306_Send_command(SSD1306_SEGREMAP | 0x01);  // column 127 mapped to SEG0
    SSD1306_Send_command(SSD1306_COMSCANDEC);       // column scan direction reversed
    SSD1306_Send_command(SSD1306_SETCOMPINS);
    SSD1306_Send_command(0x12);  // 0x12 if height > 32 else 0x02 ( если вісота больше 32 ставим 0х12 , если равно 32 или меньше то 0х02 )
    SSD1306_Send_command(SSD1306_SETCONTRAST);
    SSD1306_Send_command(0x7F);  // contrast level 127
    SSD1306_Send_command(SSD1306_SETPRECHARGE);
    SSD1306_Send_command(0xF1);  // pre-charge period (1, 15)
    SSD1306_Send_command(SSD1306_SETVCOMDETECT);
    SSD1306_Send_command(0x40);  // vcomh regulator level
    SSD1306_Send_command(SSD1306_DISPLAYALLON_RESUME);
    SSD1306_Send_command(SSD1306_NORMALDISPLAY);
    SSD1306_Send_command(SSD1306_DISPLAYON);

#elif defined SSD1306_96x16
    SSD1306_Send_command(SSD1306_DISPLAYOFF);
    SSD1306_Send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_Send_command(0x80);  // clock divide ratio and osc frequency
    SSD1306_Send_command(SSD1306_SETMULTIPLEX);
    SSD1306_Send_command(0x0F);  // multiplex ratio ratio 32 (устанавливаем размер в пикселях по высоте -1 в HEX например 0x27 = 39 ( 40-1 ) )
    SSD1306_Send_command(SSD1306_SETDISPLAYOFFSET);
    SSD1306_Send_command(0x0);                         // display offset zero
    SSD1306_Send_command(SSD1306_SETSTARTLINE | 0x0);  // set display start line to 0
    SSD1306_Send_command(SSD1306_CHARGEPUMP);
    SSD1306_Send_command(0x14);  // charge pump setting enable
    SSD1306_Send_command(SSD1306_MEMORYMODE);
    SSD1306_Send_command(0x00);                     // page addressing mode
    SSD1306_Send_command(SSD1306_SEGREMAP | 0xA1);  // segment remap
    SSD1306_Send_command(SSD1306_COMSCANDEC);       // scan dir reverse
    SSD1306_Send_command(SSD1306_SETCOMPINS);
    SSD1306_Send_command(0x02);  // 0x12 if height > 32 else 0x02 ( если вісота больше 32 ставим 0х12 , если равно 32 или меньше то 0х02 )
    SSD1306_Send_command(SSD1306_SETCONTRAST);
    SSD1306_Send_command(0xAF);  // set contrast level 0xaf
    SSD1306_Send_command(SSD1306_SETPRECHARGE);
    SSD1306_Send_command(0xF1);  // pre-charge period 0x0f1
    SSD1306_Send_command(SSD1306_SETVCOMDETECT);
    SSD1306_Send_command(0x20);                       // vcomh deselect level
    SSD1306_Send_command(SSD1306_DEACTIVATE_SCROLL);  //  Deactivate scroll
    SSD1306_Send_command(SSD1306_DISPLAYALLON_RESUME);
    SSD1306_Send_command(SSD1306_NORMALDISPLAY);
    SSD1306_Send_command(SSD1306_DISPLAYON);

#elif defined SSD1306_128x32
    SSD1306_Send_command(SSD1306_DISPLAYOFF);
    SSD1306_Send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_Send_command(0x80);  // the suggested ratio 0x80
    SSD1306_Send_command(SSD1306_SETMULTIPLEX);
    SSD1306_Send_command(0x1F);  // ratio 32 ratio 32 (устанавливаем размер в пикселях по высоте -1 в HEX например 0x27 = 39 ( 40-1 ) )
    SSD1306_Send_command(SSD1306_SETDISPLAYOFFSET);
    SSD1306_Send_command(0x0);                         // no offset
    SSD1306_Send_command(SSD1306_SETSTARTLINE | 0x0);  // line #0
    SSD1306_Send_command(SSD1306_CHARGEPUMP);
    SSD1306_Send_command(0x14);  // internal vcc
    SSD1306_Send_command(SSD1306_MEMORYMODE);
    SSD1306_Send_command(0x02);                    // page mode
    SSD1306_Send_command(SSD1306_SEGREMAP | 0x1);  // column 127 mapped to SEG0
    SSD1306_Send_command(SSD1306_COMSCANDEC);      // column scan direction reversed
    SSD1306_Send_command(SSD1306_SETCOMPINS);
    SSD1306_Send_command(0x02);  // 0x12 if height > 32 else 0x02 ( если вісота больше 32 ставим 0х12 , если равно 32 или меньше то 0х02 )
    SSD1306_Send_command(SSD1306_SETCONTRAST);
    SSD1306_Send_command(0x7F);  // contrast level 127
    SSD1306_Send_command(SSD1306_SETPRECHARGE);
    SSD1306_Send_command(0xF1);  // pre-charge period (1, 15)
    SSD1306_Send_command(SSD1306_SETVCOMDETECT);
    SSD1306_Send_command(0x40);  // vcomh regulator level
    SSD1306_Send_command(SSD1306_DISPLAYALLON_RESUME);
    SSD1306_Send_command(SSD1306_NORMALDISPLAY);
    SSD1306_Send_command(SSD1306_DISPLAYON);

#elif defined SSD1306_128x64
    SSD1306_Send_command(SSD1306_DISPLAYOFF);
    SSD1306_Send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_Send_command(0x80);  // the suggested ratio 0x80
    SSD1306_Send_command(SSD1306_SETMULTIPLEX);
    SSD1306_Send_command(0x3F);  // ratio 64 ratio 32 (устанавливаем размер в пикселях по высоте -1 в HEX например 0x27 = 39 ( 40-1 ) )
    SSD1306_Send_command(SSD1306_SETDISPLAYOFFSET);
    SSD1306_Send_command(0x0);                         // no offset
    SSD1306_Send_command(SSD1306_SETSTARTLINE | 0x0);  // line #0	// подымаем картинку вверх
    SSD1306_Send_command(SSD1306_CHARGEPUMP);
    SSD1306_Send_command(0x14);  // internal vcc
    SSD1306_Send_command(SSD1306_MEMORYMODE);
    SSD1306_Send_command(0x02);                    // page mode
    SSD1306_Send_command(SSD1306_SEGREMAP | 0x1);  // column 127 mapped to SEG0	// двигаем картинку вправо
    SSD1306_Send_command(SSD1306_COMSCANDEC);      // column scan direction reversed
    SSD1306_Send_command(SSD1306_SETCOMPINS);
    SSD1306_Send_command(0x12);  // 0x12 if height > 32 else 0x02 ( если вісота больше 32 ставим 0х12 , если равно 32 или меньше то 0х02 )
    SSD1306_Send_command(SSD1306_SETCONTRAST);
    SSD1306_Send_command(0x7F);  // contrast level 127
    SSD1306_Send_command(SSD1306_SETPRECHARGE);
    SSD1306_Send_command(0xF1);  // pre-charge period (1, 15)
    SSD1306_Send_command(SSD1306_SETVCOMDETECT);
    SSD1306_Send_command(0x40);  // vcomh regulator level
    SSD1306_Send_command(SSD1306_DISPLAYALLON_RESUME);
    SSD1306_Send_command(SSD1306_NORMALDISPLAY);
    SSD1306_Send_command(SSD1306_DISPLAYON);

#elif defined SH1106_128x64
    SSD1306_Send_command(SSD1306_DISPLAYOFF);
    SSD1306_Send_command(SSD1306_SETSTARTPAGE | 0X0);  // set page address
    SSD1306_Send_command(SSD1306_SETCONTRAST);
    SSD1306_Send_command(0x80);                    // 128 ratio 32 (устанавливаем размер в пикселях по высоте -1 в HEX например 0x27 = 39 ( 40-1 ) )
    SSD1306_Send_command(SSD1306_SEGREMAP | 0X1);  // set segment remap
    SSD1306_Send_command(SSD1306_NORMALDISPLAY);   // normal / reverse
    SSD1306_Send_command(SSD1306_SETMULTIPLEX);
    SSD1306_Send_command(0x3F);  // ratio 64
    SSD1306_Send_command(SH1106_SET_PUMP_MODE);
    SSD1306_Send_command(SH1106_PUMP_ON);                 // set charge pump enable
    SSD1306_Send_command(SH1106_SET_PUMP_VOLTAGE | 0X2);  // 8.0 volts
    SSD1306_Send_command(SSD1306_COMSCANDEC);             // Com scan direction
    SSD1306_Send_command(SSD1306_SETDISPLAYOFFSET);
    SSD1306_Send_command(0X00);  // set display offset
    SSD1306_Send_command(SSD1306_SETDISPLAYCLOCKDIV);
    SSD1306_Send_command(0X80);  // set osc division
    SSD1306_Send_command(SSD1306_SETPRECHARGE);
    SSD1306_Send_command(0X1F);  // set pre-charge period
    SSD1306_Send_command(SSD1306_SETCOMPINS);
    SSD1306_Send_command(0X12);  // 0x12 if height > 32 else 0x02 ( если вісота больше 32 ставим 0х12 , если равно 32 или меньше то 0х02 )
    SSD1306_Send_command(SSD1306_SETVCOMDETECT);
    SSD1306_Send_command(0x40);  // set vcomh
    SSD1306_Send_command(SSD1306_DISPLAYON);

#endif

    SSD1306_Send_command(SSD1306_DEACTIVATE_SCROLL);
    SSD1306_Clean_Frame_buffer();
    SSD1306_Update();

    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
    SSD1306.Initialized = 1;
    return 1;
}

/**
 ******************************************************************************
 *  @breif Функция вывода буфера кадра на дисплей
 ******************************************************************************
 */

void SSD1306_Update(void) {
    uint8_t m;

    for (m = 0; m < 8; m++) {
        SSD1306_Send_command(0xB0 + m);

#if defined(SH1106_128x64)
        // экран SH1106 имеет больше разрешение 132x64 страниц а сам экран 128 поетому смещаем на 2
        SSD1306_Send_command(SSD1306_SETLOWCOLUMN + (2 & 0x0f));
        SSD1306_Send_command(SSD1306_SETHIGHCOLUMN + (2 >> 4));
#elif defined SSD1306_72x40
        // экран SSD1306_72x40 имеет смещение в 28 пикселей, поэтому смещаем на 28 и выравниваем картинку
        SSD1306_Send_command(SSD1306_SETLOWCOLUMN + (28 & 0x0f));
        SSD1306_Send_command(SSD1306_SETHIGHCOLUMN + (28 >> 4));
#else
        SSD1306_Send_command(SSD1306_SETLOWCOLUMN);
        SSD1306_Send_command(SSD1306_SETHIGHCOLUMN);
#endif
        SSD1306_Send_data(&SSD1306_Frame_Buffer[SSD1306_WIDTH * m], SSD1306_WIDTH);
    }
}

/**
 ******************************************************************************
 *  @breif Функция очистки буфера кадра
 ******************************************************************************
 */

void SSD1306_Clean_Frame_buffer(void) {
    memset(SSD1306_Frame_Buffer, 0x00, sizeof(SSD1306_Frame_Buffer));
}

/**
 ******************************************************************************
 *  @breif Функция заполнения буфера кадра черным/белым
 ******************************************************************************
 */
void SSD1306_Fill(uint8_t color) {
    /* Set memory */
    memset(SSD1306_Frame_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SSD1306_Frame_Buffer));
}

/**
 ******************************************************************************
 *  @breif Функция рисования пикселя
 ******************************************************************************
 */

void SSD1306_Draw_pixel(uint16_t x, uint16_t y, uint8_t color) {
    if (
        x >= SSD1306_WIDTH ||
        y >= SSD1306_HEIGHT) {
        /* Error */
        return;
    }

    /* Check if pixels are inverted */
    if (SSD1306.Inverted) {
        color = (uint8_t)!color;
    }

    /* Set color */
    if (color == SSD1306_COLOR_WHITE) {
        SSD1306_Frame_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        SSD1306_Frame_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

/**
 ******************************************************************************
 *  @breif Функция рисования изображения с указанием его размера
 *  @param *bitmap - массив изображения
 *  @param x - положение по оси X
 *  @param y - положение по оси Y
 *  @param w - ширина изображения в пикселях
 *  @param h - высота изображения в пикселях
 ******************************************************************************
 */

void SSD1306_DrawBitmap(const uint8_t *bitmap, int8_t x, int8_t y, int8_t w, int8_t h) {
    int16_t byteWidth = (w + 7) / 8;  // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for (int16_t j = 0; j < h; j++, y++) {
        for (int16_t i = 0; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }

            if (byte & 0x80) {
                SSD1306_Draw_pixel(x + i, y, 1);
            }
        }
    }
}

/**
 ******************************************************************************
 *  @breif Функция вывода символа на дисплей (Шрифт 5х7)
 *  @param x - положение по оси X
 *  @param y - положение по оси Y
 *  @param symbol - символ (в таблице UTF8)
 *  @param inversion - включить/выключить инверсию изображения
 ******************************************************************************
 */

void SSD1306_Print_symbol_5x7(uint8_t x, uint8_t y, uint16_t symbol, uint8_t inversion) {
    /// Функция вывода символа на дисплей
    uint8_t x_start = x;     // начальное положение по x
    uint8_t x_stop = x + 5;  // конечное положение по x с учетом межсимвольного интервала
    for (uint8_t x = x_start; x <= x_stop; x++) {
        if (x == x_stop) {                      // Заполняем межсимвольные интервалы
            for (uint8_t i = 0; i <= 6; i++) {  // от 0 до 6, т.к. шрифт высотой 7 пикселей
                if (0x00 & (1 << i)) {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Закрасить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Закрасить пиксель
                    }
                } else {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Очистить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Очистить пиксель
                    }
                }
            }
        } else {                                // Заполняем полезной информацией
            for (uint8_t i = 0; i <= 6; i++) {  // от 0 до 6, т.к. шрифт высотой 7 пикселей
                if (Font_5x7[(symbol * 5) + x - x_start] & (1 << i)) {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Закрасить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Закрасить пиксель
                    }
                } else {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Очистить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Очистить пиксель
                    }
                }
            }
        }
    }
}

/**
 ******************************************************************************
 *  @breif Функция вывода символа на дисплей (Шрифт 3х5)
 *  @param x - положение по оси X
 *  @param y - положение по оси Y
 *  @param symbol - символ (в таблице UTF8)
 *  @param inversion - включить/выключить инверсию изображения
 ******************************************************************************
 */

void SSD1306_Print_symbol_3x5(uint8_t x, uint8_t y, uint16_t symbol, uint8_t inversion) {
    /// Функция вывода символа на дисплей
    uint8_t x_start = x;     // начальное положение по x
    uint8_t x_stop = x + 3;  // конечное положение по x с учетом межсимвольного интервала
    for (uint8_t x = x_start; x <= x_stop; x++) {
        if (x == x_stop) {                      // Заполняем межсимвольные интервалы
            for (uint8_t i = 0; i <= 4; i++) {  // от 0 до 6, т.к. шрифт высотой 7 пикселей
                if (0x00 & (1 << i)) {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Закрасить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Закрасить пиксель
                    }
                } else {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Очистить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Очистить пиксель
                    }
                }
            }
        } else {                                // Заполняем полезной информацией
            for (uint8_t i = 0; i <= 4; i++) {  // от 0 до 6, т.к. шрифт высотой 7 пикселей
                if (Font_3x5[(symbol * 3) + x - x_start] & (1 << i)) {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Закрасить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Закрасить пиксель
                    }
                } else {
                    if (inversion) {
                        SSD1306_Draw_pixel(x, y + i, 1);  // Очистить пиксель
                    } else {
                        SSD1306_Draw_pixel(x, y + i, 0);  // Очистить пиксель
                    }
                }
            }
        }
    }
}

/**
 ******************************************************************************
 *  @breif Функция декодирования UTF-8 в набор символов
 *  @param x - положение по оси X
 *  @param y - положение по оси Y
 *  @param font - выбранный шрифт
 *  @param inversion - включить/выключить инверсию изображения
 *  @param *tx_buffer - массив с тексом, который будем декодировать
 ******************************************************************************
 */

void SSD1306_Decode_UTF8(uint8_t x, uint8_t y, uint8_t font, bool inversion, char *tx_buffer) {
    uint16_t symbol = 0;
    bool flag_block = 0;
    for (int i = 0; i < strlen(tx_buffer); i++) {
        if (tx_buffer[i] < 0xC0) {  // Английский текст и символы. Если до русского текста, то [i] <0xD0. Но в font добавлен знак "°"
            if (flag_block) {
                flag_block = 0;
            } else {
                symbol = tx_buffer[i];
                if (font == FONT_3x5) {  // Если выбран шрифт размера 3x5
                    if (inversion) {
                        SSD1306_Print_symbol_3x5(x, y, symbol - 32, 1);  // Таблица UTF-8. Basic Latin. С "пробел" до "z". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_3x5(x, y, symbol - 32, 0);  // Таблица UTF-8. Basic Latin. С "пробел" до "z". Инверсия выкл.
                    }
                    x = x + 4;
                } else if (font == FONT_5x7) {  // Если выбран шрифт размера 5x7
                    if (inversion) {
                        SSD1306_Print_symbol_5x7(x, y, symbol - 32, 1);  // Таблица UTF-8. Basic Latin. С "пробел" до "z". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_5x7(x, y, symbol - 32, 0);  // Таблица UTF-8. Basic Latin. С "пробел" до "z". Инверсия выкл.
                    }
                    x = x + 6;
                }
            }
        }

        else {  // Русский текст
            symbol = tx_buffer[i] << 8 | tx_buffer[i + 1];
            if (symbol < 0xD180 && symbol > 0xD081) {
                if (font == FONT_3x5) {  // Если выбран шрифт размера 3x5
                    if (inversion) {
                        SSD1306_Print_symbol_3x5(x, y, symbol - 53297, 1);  // Таблица UTF-8. Кириллица. С буквы "А" до "п". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_3x5(x, y, symbol - 53297, 0);  // Таблица UTF-8. Кириллица. С буквы "А" до "п". Инверсия выкл.
                    }
                    x = x + 4;
                } else if (font == FONT_5x7) {  // Если выбран шрифт размера 5x7
                    if (inversion) {
                        SSD1306_Print_symbol_5x7(x, y, symbol - 53297, 1);  // Таблица UTF-8. Кириллица. С буквы "А" до "п". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_5x7(x, y, symbol - 53297, 0);  // Таблица UTF-8. Кириллица. С буквы "А" до "п". Инверсия выкл.
                    }
                    x = x + 6;
                }
            } else if (symbol == 0xD081) {
                if (font == FONT_3x5) {  // Если выбран шрифт размера 3x5
                    if (inversion) {
                        SSD1306_Print_symbol_3x5(x, y, 159, 1);  ////Таблица UTF-8. Кириллица. Буква "Ё". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_3x5(x, y, 159, 0);  ////Таблица UTF-8. Кириллица. Буква "Ё". Инверсия выкл.
                    }
                    x = x + 4;
                } else if (font == FONT_5x7) {  // Если выбран шрифт размера 5x7
                    if (inversion) {
                        SSD1306_Print_symbol_5x7(x, y, 159, 1);  ////Таблица UTF-8. Кириллица. Буква "Ё". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_5x7(x, y, 159, 0);  ////Таблица UTF-8. Кириллица. Буква "Ё". Инверсия выкл.
                    }
                    x = x + 6;
                }
            } else if (symbol == 0xD191) {
                if (font == FONT_3x5) {  // Если выбран шрифт размера 3x5
                    if (inversion) {
                        SSD1306_Print_symbol_3x5(x, y, 160, 1);  ////Таблица UTF-8. Кириллица. Буква "ё". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_3x5(x, y, 160, 0);  ////Таблица UTF-8. Кириллица. Буква "ё". Инверсия выкл.
                    }
                    x = x + 4;
                } else if (font == FONT_5x7) {  // Если выбран шрифт размера 5x7
                    if (inversion) {
                        SSD1306_Print_symbol_5x7(x, y, 160, 1);  ////Таблица UTF-8. Кириллица. Буква "ё". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_5x7(x, y, 160, 0);  ////Таблица UTF-8. Кириллица. Буква "ё". Инверсия выкл.
                    }
                    x = x + 6;
                }
            } else if (symbol == 0xC2B0) {
                if (font == FONT_3x5) {  // Если выбран шрифт размера 3x5
                    if (inversion) {
                        SSD1306_Print_symbol_3x5(x, y, 161, 1);  ////Таблица UTF-8. Basic Latin. Символ "°". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_3x5(x, y, 161, 0);  ////Таблица UTF-8. Basic Latin. Символ "°". Инверсия выкл.
                    }
                    x = x + 4;
                } else if (font == FONT_5x7) {  // Если выбран шрифт размера 5x7
                    if (inversion) {
                        SSD1306_Print_symbol_5x7(x, y, 161, 1);  ////Таблица UTF-8. Basic Latin. Символ "°". Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_5x7(x, y, 161, 0);  ////Таблица UTF-8. Basic Latin. Символ "°". Инверсия выкл.
                    }
                    x = x + 6;
                }
            }

            else {
                if (font == FONT_3x5) {  // Если выбран шрифт размера 3x5
                    if (inversion) {
                        SSD1306_Print_symbol_3x5(x, y, symbol - 53489, 1);  // Таблица UTF-8. Кириллица. С буквы "р", начинается сдвиг. Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_3x5(x, y, symbol - 53489, 0);  // Таблица UTF-8. Кириллица. С буквы "р", начинается сдвиг. Инверсия выкл.
                    }
                    x = x + 4;
                } else if (font == FONT_5x7) {  // Если выбран шрифт размера 5x7
                    if (inversion) {
                        SSD1306_Print_symbol_5x7(x, y, symbol - 53489, 1);  // Таблица UTF-8. Кириллица. С буквы "р", начинается сдвиг. Инверсия вкл.
                    } else {
                        SSD1306_Print_symbol_5x7(x, y, symbol - 53489, 0);  // Таблица UTF-8. Кириллица. С буквы "р", начинается сдвиг. Инверсия выкл.
                    }
                    x = x + 6;
                }
            }
            flag_block = 1;
        }
    }
}

/**
 ******************************************************************************
 *  @breif Функция управления контрастом изображения
 *  @param  value - значение 0-255
 ******************************************************************************
 */

void SSD1306_Contrast(uint8_t value) {
    SSD1306_Send_command(0x81);
    SSD1306_Send_command(value);
}

/**
 ******************************************************************************
 *  @breif Функция включения инверсии всего изображения
 *  @param  i - вкл/выкл инверсию
 ******************************************************************************
 */

void SSD1306_Inversion(int i) {
    if (i)
        SSD1306_Send_command(SSD1306_INVERTDISPLAY);

    else
        SSD1306_Send_command(SSD1306_NORMALDISPLAY);
}

/**
 ******************************************************************************
 *  @breif Функция, формирующая точку значения на графике
 *  @param y_min - минимальное значение по оси y
 *  @param y_max - максимальное значение по оси y
 *  @param value - значение, которое будем отображать на графике
 ******************************************************************************
 */

uint8_t SSD1306_Value_for_Plot(int y_min, int y_max, float value) {
    // clang-format off
    char text[20] = {0,};
    // clang-format on
    uint8_t Graph_value = 0;
    if (y_max > y_min && value >= y_min && value <= y_max) {
        float y = 50.0f - (y_max + value * (-1)) * (50.0f / (y_max + y_min * (-1)));
        Graph_value = (uint8_t)y;
    } else if (value > y_max) {
        Graph_value = 50;
        sprintf(text, "   Clipping    ");
        SSD1306_Decode_UTF8(37, 26, 1, 0, text);
        SSD1306_Update();
    } else if (value < y_min) {
        Graph_value = 0;
        sprintf(text, "   Clipping    ");
        SSD1306_Decode_UTF8(37, 26, 1, 0, text);
        SSD1306_Update();
    }
    return Graph_value;
}

/**
 ******************************************************************************
 *  @breif Функция, заполняющая массив значениями, чтоб отрисовывать график
 *   @param uint8_t *counter - счетчик значений
 *   @param uint8_t *array - массив, куда будем закидывать значения для отображения графика
 *   @param bool *array_is_full - значения заполнили массив, можно сдвигать график влево
 *   @param uint8_t size_array - размер массива, куда будем закидывать значения для отображения графика
 *   @param uint8_t value - подготовленное значение для графика(подготавливается при помощи ф-ии uint8_t SSD1306_Value_for_Plot(int y_min, int y_max, float value))
 ******************************************************************************
 */

void SSD1306_Fill_the_array_Plot(uint8_t *counter, uint8_t *array, uint8_t size_array, bool *array_is_full, uint8_t value) {
    if (*counter == 0) {
        array[0] = value;
    }

    if (*counter) {
        if (*counter <= size_array - 1) {
            for (int i = 0; i <= *counter; i++) {
                if (i == *counter) {
                    array[0] = value;
                } else {
                    array[*counter - i] = array[*counter - i - 1];
                }
            }
        } else if (*counter > size_array - 1) {
            for (int i = size_array - 1; i >= 0; i--) {
                array[i] = array[i - 1];
                if (i == 0) {
                    array[i] = value;
                }
            }
        }
    }
    (*counter)++;
    if (*counter == 250) {
        *counter = 128;  // Защита от переполнения, иначе график будет сбрасываться(главное чтоб разница была равна или больше кол-ву выводимых точек)
    }

    if (*counter <= size_array) {
        if (*counter == size_array) {
            *array_is_full = true;
        }
    }
}

/**
 ******************************************************************************
 *  @breif Функция, формирующая весь график, который будет отображаться на дисплее
 *  @param uint8_t *counter - счетчик значений
 *  @param uint8_t *array - массив, куда мы закидывали значения для отображения графика
 *  @param uint8_t size_array - размер массива, куда мы закидывали значения для отображения графика
 *  @param bool *array_is_full - значения заполнили массив, можно сдвигать график влево
 *  @param int y_min - минимальное значение по оси y
 *  @param int y_max - максимальное значение по оси y
 *  @param uint8_t x_grid_time - размер клетки по x(Например, график обновляется раз в 100 мс, значит одна клетка по X будет 1 сек.)
 *  @param uint8_t time_interval - интервал времени. 0 - сек., 1 - мин., 2 = час.
 *  @param bool grid - Сетка граффика. вкл = 1. выкл = 0.
 ******************************************************************************
 */

void SSD1306_Generate_a_Graph(uint8_t *counter, uint8_t *array, uint8_t size_array, bool *array_is_full, int y_min, int y_max, uint8_t x_grid_time, uint8_t time_interval, bool grid) {
    int y_minimum = y_min;
    int y_maximum = y_max;
    if (y_min < 0) {
        y_minimum = y_minimum * (-1);
    }
    if (y_max < 0) {
        y_maximum = y_maximum * (-1);
    }
    int val_del = (y_minimum + y_maximum) / 5;
    char text[20] = {
        0,
    };
    SSD1306_Clean_Frame_buffer();
    sprintf(text, "%4.1d", y_min);
    SSD1306_Decode_UTF8(8, 48, 0, 0, text);
    sprintf(text, "%4.1d", (y_min + val_del));
    SSD1306_Decode_UTF8(8, 38, 0, 0, text);
    sprintf(text, "%4.1d", (y_min + val_del * 2));
    SSD1306_Decode_UTF8(8, 28, 0, 0, text);
    sprintf(text, "%4.1d", (y_min + val_del * 3));
    SSD1306_Decode_UTF8(8, 18, 0, 0, text);
    sprintf(text, "%4.1d", (y_min + val_del * 4));
    SSD1306_Decode_UTF8(8, 8, 0, 0, text);
    sprintf(text, "%4.1d", y_max);
    SSD1306_Decode_UTF8(8, 0, 0, 0, text);

    if (time_interval == 0) {
        sprintf(text, "%2.1dcек.", x_grid_time);
    } else if (time_interval == 1) {
        sprintf(text, "%2.1dмин.", x_grid_time);
    } else if (time_interval == 2) {
        sprintf(text, "%2.1dчас.", x_grid_time);
    }
    SSD1306_Decode_UTF8(80, 57, 1, 0, text);  // 53

    /*----------------Ось асцисс, ось ординат, разметка-----------------*/

    SSD1306_Draw_line(27, 50, 127, 50, 1);  // ось абсцисс
    for (uint8_t i = 27; i <= 77; i = i + 10) {
        SSD1306_Draw_line(i, 51, i, 52, 1);
    }
    for (uint8_t i = 87; i <= 127; i = i + 10) {
        SSD1306_Draw_pixel(i, 51, 1);
    }

    SSD1306_Draw_line(27, 0, 27, 50, 1);  // ось ординат
    for (uint8_t i = 0; i <= 50; i = i + 10) {
        SSD1306_Draw_line(25, i, 27, i, 1);
    }

    // 57 67
    SSD1306_Draw_line(87, 53, 87, 55, 1);    // Полоса по х левая
    SSD1306_Draw_line(97, 53, 97, 55, 1);    // Полоса по х правая
    SSD1306_Draw_line(82, 54, 102, 54, 1);   // Полоса между делениями по х
    SSD1306_Draw_line(84, 52, 84, 56, 1);    // Левая стрелка на деление
    SSD1306_Draw_line(85, 53, 85, 55, 1);    // Левая стрелка на деление
    SSD1306_Draw_line(99, 53, 99, 55, 1);    // Правая стрелка на деление
    SSD1306_Draw_line(100, 52, 100, 56, 1);  // Правая стрелка на деление
    SSD1306_Draw_line(127, 53, 127, 57, 1);  // Отметка "сейчас"
    SSD1306_Draw_line(122, 55, 127, 55, 1);  // Стрелка на "сейчас"
    SSD1306_Draw_line(124, 53, 124, 57, 1);  // Стрелка на "сейчас"
    SSD1306_Draw_line(125, 54, 125, 56, 1);  // Стрелка на "сейчас"

    /*----------------Ось асцисс, ось ординат, разметка-----------------*/

    /*--------------------------Разметка сетки-------------------------*/
    if (grid) {
        for (uint8_t y = 0; y <= 40; y = y + 10) {
            for (uint8_t x = 27; x <= 127; x = x + 2) {
                SSD1306_Draw_pixel(x, y, 1);
            }
        }

        for (uint8_t x = 27; x <= 127; x = x + 10) {
            for (uint8_t y = 0; y <= 50; y = y + 2) {
                SSD1306_Draw_pixel(x, y, 1);
            }
        }
    }
    /*--------------------------Разметка сетки-------------------------*/

    if (*array_is_full) {
        for (int i = 0; i < size_array - 1; i++) {
            // SSD1306_Draw_pixel(127 - i, 50 - array[i], 1); //Рисуем точками
            SSD1306_Draw_line(127 - (i + 1), 50 - array[i + 1], 127 - i, 50 - array[i], 1);  // Рисуем линиями
            // SSD1306_Draw_line(127-i, 50, 127-i, 50 - array[i], 1);//Закрашиваем область
        }
    } else {
        for (int i = 0; i < *counter; i++) {
            // SSD1306_Draw_pixel(127 - i, 50 - array[i], 1); //Рисуем точками
            SSD1306_Draw_line(127 - (i + 1), 50 - array[i + 1], 127 - i, 50 - array[i], 1);  // Рисуем линиями
            // SSD1306_Draw_line(127-i, 50, 127-i, 50 - array[i], 1);//Закрашиваем область
        }
    }
}

/********************************РАБОТА С ГЕОМЕТРИЧЕСКИМИ ФИГУРАМИ**********************************/

void SSD1306_Draw_line(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color) {
    int dx = (x_end >= x_start) ? x_end - x_start : x_start - x_end;
    int dy = (y_end >= y_start) ? y_end - y_start : y_start - y_end;
    int sx = (x_start < x_end) ? 1 : -1;
    int sy = (y_start < y_end) ? 1 : -1;
    int err = dx - dy;

    for (;;) {
        SSD1306_Draw_pixel(x_start, y_start, color);
        if (x_start == x_end && y_start == y_end)
            break;
        int e2 = err + err;
        if (e2 > -dy) {
            err -= dy;
            x_start += sx;
        }
        if (e2 < dx) {
            err += dx;
            y_start += sy;
        }
    }
}

/*--------------------------------Вывести пустотелый прямоугольник---------------------------------*/
void SSD1306_Draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
    /// Вывести пустотелый прямоугольник
    /// \param x - начальная точка по оси "x"
    /// \param y - начальная точка по оси "y"
    /// \param width - ширина прямоугольника
    /// \param height - высота прямоугольника

    /*Проверка ширины и высоты*/
    if ((x + width) >= SSD1306_WIDTH) {
        width = SSD1306_WIDTH - x;
    }
    if ((y + height) >= SSD1306_HEIGHT) {
        height = SSD1306_HEIGHT - y;
    }

    /*Рисуем линии*/
    SSD1306_Draw_line(x, y, x + width, y, color);                   /*Верх прямоугольника*/
    SSD1306_Draw_line(x, y + height, x + width, y + height, color); /*Низ прямоугольника*/
    SSD1306_Draw_line(x, y, x, y + height, color);                  /*Левая сторона прямоугольника*/
    SSD1306_Draw_line(x + width, y, x + width, y + height, color);  /*Правая сторона прямоугольника*/
}
/*--------------------------------Вывести пустотелый прямоугольник---------------------------------*/

/*-------------------------------Вывести закрашенный прямоугольник---------------------------------*/
void SSD1306_Draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
    /// Вывести закрашенный прямоугольник
    /// \param x - начальная точка по оси "x"
    /// \param y - начальная точка по оси "y"
    /// \param width - ширина прямоугольника
    /// \param height - высота прямоугольника

    /*Проверка ширины и высоты*/
    if ((x + width) >= SSD1306_WIDTH) {
        width = SSD1306_WIDTH - x;
    }
    if ((y + height) >= SSD1306_HEIGHT) {
        height = SSD1306_HEIGHT - y;
    }

    /*Рисуем линии*/
    for (uint8_t i = 0; i <= height; i++) {
        SSD1306_Draw_line(x, y + i, x + width, y + i, color);
    }
}
/*-------------------------------Вывести закрашенный прямоугольник---------------------------------*/

/*---------------------------------Вывести пустотелую окружность-----------------------------------*/
void SSD1306_Draw_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color) {
    /// Вывести пустотелую окружность
    /// \param x - точка центра окружности по оси "x"
    /// \param y - точка центра окружности по оси "y"
    /// \param radius - радиус окружности

    int f = 1 - (int)radius;
    int ddF_x = 1;

    int ddF_y = -2 * (int)radius;
    int x_0 = 0;

    SSD1306_Draw_pixel(x, y + radius, color);
    SSD1306_Draw_pixel(x, y - radius, color);
    SSD1306_Draw_pixel(x + radius, y, color);
    SSD1306_Draw_pixel(x - radius, y, color);

    int y_0 = radius;
    while (x_0 < y_0) {
        if (f >= 0) {
            y_0--;
            ddF_y += 2;
            f += ddF_y;
        }
        x_0++;
        ddF_x += 2;
        f += ddF_x;
        SSD1306_Draw_pixel(x + x_0, y + y_0, color);
        SSD1306_Draw_pixel(x - x_0, y + y_0, color);
        SSD1306_Draw_pixel(x + x_0, y - y_0, color);
        SSD1306_Draw_pixel(x - x_0, y - y_0, color);
        SSD1306_Draw_pixel(x + y_0, y + x_0, color);
        SSD1306_Draw_pixel(x - y_0, y + x_0, color);
        SSD1306_Draw_pixel(x + y_0, y - x_0, color);
        SSD1306_Draw_pixel(x - y_0, y - x_0, color);
    }
}
/*---------------------------------Вывести пустотелую окружность-----------------------------------*/

/*--------------------------------Вывести закрашенную окружность-----------------------------------*/
void SSD1306_Draw_circle_filled(int16_t x, int16_t y, int16_t radius, uint8_t color) {
    /// Вывести закрашенную окружность
    /// \param x - точка центра окружности по оси "x"
    /// \param y - точка центра окружности по оси "y"
    /// \param radius - радиус окружности

    int16_t f = 1 - radius;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x_0 = 0;
    int16_t y_0 = radius;

    SSD1306_Draw_pixel(x, y + radius, color);
    SSD1306_Draw_pixel(x, y - radius, color);
    SSD1306_Draw_pixel(x + radius, y, color);
    SSD1306_Draw_pixel(x - radius, y, color);
    SSD1306_Draw_line(x - radius, y, x + radius, y, color);

    while (x_0 < y_0) {
        if (f >= 0) {
            y_0--;
            ddF_y += 2;
            f += ddF_y;
        }
        x_0++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_Draw_line(x - x_0, y + y_0, x + x_0, y + y_0, color);
        SSD1306_Draw_line(x + x_0, y - y_0, x - x_0, y - y_0, color);
        SSD1306_Draw_line(x + y_0, y + x_0, x - y_0, y + x_0, color);
        SSD1306_Draw_line(x + y_0, y - x_0, x - y_0, y - x_0, color);
    }
}
/*--------------------------------Вывести закрашенную окружность-----------------------------------*/

/*-----------------------------------Вывести пустотелый треугольник--------------------------------*/
void SSD1306_Draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color) {
    /// Вывести пустотелый треугольник
    /// \param x_1 - первая точка треугольника. Координата по оси "x"
    /// \param y_1 - первая точка треугольника. Координата по оси "y"
    /// \param x_2 - вторая точка треугольника. Координата по оси "x"
    /// \param y_2 - вторая точка треугольника. Координата по оси "y"
    /// \param x_3 - третья точка треугольника. Координата по оси "x"
    /// \param y_3 - третья точка треугольника. Координата по оси "y"

    SSD1306_Draw_line(x1, y1, x2, y2, color);
    SSD1306_Draw_line(x2, y2, x3, y3, color);
    SSD1306_Draw_line(x3, y3, x1, y1, color);
}
/*-----------------------------------Вывести пустотелый треугольник--------------------------------*/

/*----------------------------------Вывести закрашенный треугольник--------------------------------*/
void SSD1306_Draw_triangle_filled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color) {
    /// Вывести закрашенный треугольник
    /// \param x_1 - первая точка треугольника. Координата по оси "x"
    /// \param y_1 - первая точка треугольника. Координата по оси "y"
    /// \param x_2 - вторая точка треугольника. Координата по оси "x"
    /// \param y_2 - вторая точка треугольника. Координата по оси "y"
    /// \param x_3 - третья точка треугольника. Координата по оси "x"
    /// \param y_3 - третья точка треугольника. Координата по оси "y"

#define ABS(x) ((x) > 0 ? (x) : -(x))
    int16_t deltax = 0;
    int16_t deltay = 0;
    int16_t x = 0;
    int16_t y = 0;
    int16_t xinc1 = 0;
    int16_t xinc2 = 0;
    int16_t yinc1 = 0;
    int16_t yinc2 = 0;
    int16_t den = 0;
    int16_t num = 0;
    int16_t numadd = 0;
    int16_t numpixels = 0;
    int16_t curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    } else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    } else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) {
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        SSD1306_Draw_line(x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}
/*----------------------------------Вывести закрашенный треугольник--------------------------------*/

/********************************РАБОТА С ГЕОМЕТРИЧЕСКИМИ ФИГУРАМИ**********************************/
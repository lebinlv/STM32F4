#ifndef OLED_FONT_H_
#define OLED_FONT_H_

#include "stdint.h"

struct fontStruct{
    uint8_t fontWidth;
    uint8_t fontHeight;
    uint8_t firstChar;
    uint8_t totalChar;
    uint8_t *jumpTable;
    uint8_t *fontData;
};


#endif
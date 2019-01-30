#ifndef __UTIL_H__
#define __UTIL_H__

#include <Arduino.h>

typedef struct RgbColor_t{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RgbColor;

typedef struct HsvColor_t{
    uint8_t h;
    uint8_t s;
    uint8_t v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv);

HsvColor RgbToHsv(RgbColor rgb);

RgbColor EnhanceColor(RgbColor rgb);

void printHsv(HsvColor hsv);

void printRgb(RgbColor rgb);

#endif /*__UTIL_H__*/

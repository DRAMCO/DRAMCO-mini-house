#ifndef __COLOR_SENSOR__
#define __COLOR_SENSOR__

#include <Arduino.h>
#include "Adafruit_TCS34725.h" // (RGB color sensor)
#include "util.h"

class ColorSensor {
public:
  ColorSensor();

  bool begin(void);

  RgbColor getRgb(void);

protected:
  Adafruit_TCS34725 tcs;
};


#endif /*__COLOR_SENSOR__*/

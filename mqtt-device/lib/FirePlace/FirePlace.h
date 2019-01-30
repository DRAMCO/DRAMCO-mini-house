#ifndef __FIRE_PLACE__
#define __FIRE_PLACE__

#include <Arduino.h>
#include "FastLED.h"
#include "util.h"

extern "C" {
  #include "user_interface.h"
}

#define LED_PIN     12
#define NUM_LEDS    5
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB


#define UPDATES_PER_SECOND 100

class FirePlace {
public:
  FirePlace();

  void begin(void);
  void draw(void);
  void setFireColor(RgbColor rgb);
  RgbColor getFireColor(void);
private:
  void AddColor(uint8_t position, RgbColor color);
  void SubtractColor(uint8_t position, RgbColor color);
  RgbColor Blend(RgbColor color1, RgbColor color2);
  RgbColor Subtract(RgbColor color1, RgbColor color2);

  RgbColor fire_color;
protected:
  CRGB leds[NUM_LEDS];
};


#endif /*__FIRE_PLACE__*/

#include "FirePlace.h"


os_timer_t fireTimer;

uint8_t rDelay, tCount;

static void fireTimerCallback(void *pArg){
  if(tCount++ == rDelay){
    tCount = 0;
    rDelay = random(5, 15);
  }
  FirePlace* op = (FirePlace *) pArg;
  op->draw();
};

FirePlace::FirePlace(){
  for(int i=0; i<NUM_LEDS; i++){
    this->leds[i] = CRGB::Black;
  }

  fire_color.r = 0xbf;
  fire_color.g = 0x78;
  fire_color.b = 0x07;

  rDelay = random(5, 15);
  tCount = 0;
  os_timer_setfn(&fireTimer, &fireTimerCallback, (void*)this);
}

void FirePlace::begin(void){
  Serial.println(" - FastLED init.");
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  os_timer_arm(&fireTimer, 10, true);
  FastLED.show();
}

void FirePlace::draw(void){
  //Clear();

  for(int i=0; i<NUM_LEDS; i++){
    AddColor(i, fire_color);
    int r = random(80);
    RgbColor diff_color = (RgbColor){.r = r, .g = r/2, .b = r/2};
    SubtractColor(i, diff_color);
  }

  FastLED.show();
}

void FirePlace::setFireColor(RgbColor rgb){
  this->fire_color.r = rgb.r;
  this->fire_color.g = rgb.g;
  this->fire_color.b = rgb.b;
}

RgbColor FirePlace::getFireColor(void){
  RgbColor rgb;
  rgb.r = this->fire_color.r;
  rgb.g = this->fire_color.g;
  rgb.b = this->fire_color.b;
  return rgb;
}

///
/// Set color of LED
///
void FirePlace::AddColor(uint8_t position, RgbColor color){
  RgbColor old_color = (RgbColor){.r = this->leds[position].r, .g = this->leds[position].g, .b = this->leds[position].b};
  RgbColor blended_color = Blend(old_color, color);
  this->leds[position].setRGB(blended_color.r, blended_color.g, blended_color.b);
}

///
/// Set color of LED
///
void FirePlace::SubtractColor(uint8_t position, RgbColor color){
  RgbColor old_color = (RgbColor){.r = this->leds[position].r, .g = this->leds[position].g, .b = this->leds[position].b};
  RgbColor blended_color = Subtract(old_color, color);
  this->leds[position].setRGB(blended_color.r, blended_color.g, blended_color.b);
}

///
/// Color blending
///
RgbColor FirePlace::Blend(RgbColor color1, RgbColor color2){
  RgbColor newColor;
  newColor.r = constrain(color1.r+color2.r, 0, 255);
  newColor.g = constrain(color1.g+color2.g, 0, 255);
  newColor.b = constrain(color1.b+color2.b, 0, 255);
  return newColor;
}

///
/// Color blending
///
RgbColor FirePlace::Subtract(RgbColor color1, RgbColor color2){
  int16_t r, g, b;

  r=(int16_t)color1.r-(int16_t)color2.r;
  g=(int16_t)color1.g-(int16_t)color2.g;
  b=(int16_t)color1.b-(int16_t)color2.b;
  if(r<0) r=0;
  if(g<0) g=0;
  if(b<0) b=0;

  RgbColor newColor;
  newColor.r = (uint8_t) r;
  newColor.g = (uint8_t) g;
  newColor.b = (uint8_t) b;
  return newColor;
}

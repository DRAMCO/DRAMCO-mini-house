#include "ColorSensor.h"

ColorSensor::ColorSensor(){
  this->tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
}

bool ColorSensor::begin(void){
  Serial.println(" - TCSG34725 init");

  // initialize sensor
  if(!this->tcs.begin()){
    return false;
  }

  return true;
}

RgbColor ColorSensor::getRgb(void){
  uint16_t clear, red, green, blue;

  this->tcs.setInterrupt(false);      // turn on LED
  delay(60);  // takes 50ms to read
  this->tcs.getRawData(&red, &green, &blue, &clear);
  this->tcs.setInterrupt(true);  // turn off LED

  RgbColor rgb = (RgbColor){.r = (red>>6), .g = (green>>6), .b = (blue>>6)};
  return rgb;
}

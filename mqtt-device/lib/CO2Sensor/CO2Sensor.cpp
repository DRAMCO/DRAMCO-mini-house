#include <Wire.h>
#include "CO2Sensor.h"

CO2Sensor::CO2Sensor(){

}

bool CO2Sensor::begin(void){
  Serial.println(" - iAQ-Core init.");
  Wire.begin();
  Wire.setClockStretchLimit(1000); // Default is 230us, see line78 of https://github.com/esp8266/Arduino/blob/master/cores/esp8266/core_esp8266_si2c.c

  if(!iaqcore.begin()){
    return false;
  }

  return true;
}

uint16_t CO2Sensor::getCo2(void){
  // Read air quality
  uint16_t eco2;
  uint16_t stat;
  uint32_t resist;
  uint16_t etvoc;
  iaqcore.read(&eco2,&stat,&resist,&etvoc);

  // Print
  if( stat & IAQCORE_STAT_I2CERR ) {
    Serial.println("iAQcore: I2C error");
  } else if( stat & IAQCORE_STAT_ERROR ) {
    Serial.println("iAQcore: chip broken");
  } else if( stat & IAQCORE_STAT_BUSY ) {
    Serial.println("iAQcore: chip busy");
  } else {
    Serial.print("iAQcore: ");
    Serial.print("co2=");    Serial.print(eco2);     Serial.print(" ppm,  ");
    if( stat&IAQCORE_STAT_RUNIN ) Serial.print(" RUNIN");
    Serial.println();
  }

  return eco2;
}

#include <Wire.h>
#include "CO2Sensor.h"

CO2Sensor::CO2Sensor(){

}

bool CO2Sensor::begin(uint8_t sda, uint8_t scl, uint8_t led){
  Serial.println(" - iAQ-Core init.");
  Serial.print(" - I2C pins: (SDA = GPIO");
  Serial.print(sda);
  Serial.print(") ; (SCL = GPIO");
  Serial.print(scl);
  Serial.println(")");
  Serial.print(" - Warning LED on GPIO");
  Serial.println(led);
  Serial.print(" - Warning level at ");
  Serial.print(DEFAULT_TRESHOLD);
  Serial.println(" ppm");

  this->_co2Treshold = DEFAULT_TRESHOLD;

  this->_led = led;
  pinMode(this->_led, OUTPUT);
  digitalWrite(this->_led, LOW);

  Wire.begin(sda, scl);
  Wire.setClockStretchLimit(1000); // Default is 230us, see line78 of https://github.com/esp8266/Arduino/blob/master/cores/esp8266/core_esp8266_si2c.c

  if(!iaqcore.begin()){
    return false;
  }

  return true;
}

uint16_t CO2Sensor::getCo2(void){
  // toggle led
  digitalWrite(this->_led, !digitalRead(this->_led));

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

  if(eco2 > this->_co2Treshold){
    digitalWrite(this->_led, HIGH);
  }
  else{
    digitalWrite(this->_led, LOW);
  }

  return eco2;
}

uint16_t CO2Sensor::setWarningLevel(uint16_t treshold){
  this->_co2Treshold = treshold;
}

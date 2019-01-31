#ifndef __CO2_SENSOR__
#define __CO2_SENSOR__

#include <Arduino.h>
#include "iAQcore.h" // iAQ-Core driver (air quality sensor)

#define DEFAULT_SDA_PIN D6
#define DEFAULT_SCL_PIN D5
#define DEFAULT_LED_PIN D3

#define DEFAULT_TRESHOLD 5000

class CO2Sensor{
public:
  CO2Sensor();
  bool begin(uint8_t sda = DEFAULT_SDA_PIN, uint8_t scl = DEFAULT_SCL_PIN, uint8_t led = DEFAULT_LED_PIN);

  uint16_t setWarningLevel(uint16_t treshold);
  uint16_t getCo2(void);

protected:
  iAQcore iaqcore;
  uint8_t _led;
  uint16_t _co2Treshold;
};

#endif

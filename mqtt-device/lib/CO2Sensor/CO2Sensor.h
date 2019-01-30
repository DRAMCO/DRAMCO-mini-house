#ifndef __CO2_SENSOR__
#define __CO2_SENSOR__

#include <Arduino.h>
#include "iAQcore.h" // iAQ-Core driver (air quality sensor)

class CO2Sensor{
public:
  CO2Sensor();
  bool begin(void);

  uint16_t getCo2(void);

protected:
  iAQcore iaqcore;
};

#endif

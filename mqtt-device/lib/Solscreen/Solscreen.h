#ifndef __SOLSCREEN_H__
#define __SOLSCREEN_H

#include <Arduino.h>

extern "C" {
  #include "user_interface.h"
}

#define DEFAULT_DIRPIN1   14
#define DEFAULT_DIRPIN2   12
#define DEFAULT_PWMPIN    13
#define DEFAULT_DURATION  3000
#define DUTY_CYCLE        800

typedef enum statuses{CURTAIN_UP, GOING_UP, CURTAIN_DOWN, GOING_DOWN, STATE_ERROR, STATE_INIT} ScreenStatus_t;

typedef void (*Callback_t)();

class Solscreen{
public:
  Solscreen(uint8_t pwmPin = DEFAULT_PWMPIN,
    uint8_t dirPin1 = DEFAULT_DIRPIN1,
    uint8_t dirPin2 = DEFAULT_DIRPIN2,
    uint16_t duration = DEFAULT_DURATION);
  void begin(Callback_t cb = NULL);
  void setDuration(uint16_t duration);
  void goUp(void);
  void goDown(void);
  void stop(void);
  ScreenStatus_t getStatus(void);

protected:
  uint16_t _duration;
  uint8_t _motorDirPin1;
  uint8_t _motorDirPin2;
  uint8_t _motorPwmPin;
  bool _running;
  ScreenStatus_t _status;
};

#endif

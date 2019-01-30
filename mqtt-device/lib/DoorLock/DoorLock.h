#ifndef __DOOR_LOCK_H__
#define __DOOR_LOCK_H__

#include <Arduino.h>

#define DOORPIN 13

typedef enum doorstatuses{LOCK_OPEN, LOCK_CLOSED} DoorStatus_t;

class DoorLock{
public:
  DoorLock(uint8_t pin = DOORPIN);
  void begin(void);
  void open(void);
  void close(void);
  void toggle(void);
  DoorStatus_t status(void);

private:
  uint8_t _pin;
};

#endif /*__DOOR_LOCK_H__*/

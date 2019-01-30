#include "DoorLock.h"


DoorLock::DoorLock(uint8_t pin){
  _pin = pin;
}

void DoorLock::begin(void){
  Serial.print(" - Using pin GPIO");
  Serial.print(_pin);
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void DoorLock::open(void){
  digitalWrite(_pin, HIGH);
}

void DoorLock::close(void){
  digitalWrite(_pin, LOW);
}

void DoorLock::toggle(void){
  digitalWrite(_pin, !digitalRead(_pin));
}

DoorStatus_t DoorLock::status(void){
  if(digitalRead(_pin)==HIGH){
    return LOCK_OPEN;
  }
  return LOCK_CLOSED;
}

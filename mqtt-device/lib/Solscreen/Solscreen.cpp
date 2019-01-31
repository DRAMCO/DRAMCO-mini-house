#include "Solscreen.h"

os_timer_t runTimer;

Callback_t user_cb;

static void runTimerCallback(void *pArg){
  Solscreen* op = (Solscreen *) pArg;
  op->stop();
  Serial.println("Motor stopped");
  user_cb();
};

Solscreen::Solscreen(uint8_t pwmPin, uint8_t dirPin1, uint8_t dirPin2, uint16_t duration){
  // init attributes
  this->_motorPwmPin = pwmPin;
  this->_motorDirPin1 = dirPin1;
  this->_motorDirPin2 = dirPin2;
  this->_duration = duration;

  os_timer_setfn(&runTimer, &runTimerCallback, (void*)this);

  this->_status = STATE_INIT;
}

void Solscreen::begin(Callback_t cb){
  Serial.println(" - Using pins:");
  Serial.print("     GPIO");
  Serial.print(this->_motorPwmPin);
  Serial.println(" (pwm)");
  Serial.print("     GPIO");
  Serial.print(this->_motorDirPin1);
  Serial.println(" (dir1)");
  Serial.print("     GPIO");
  Serial.print(this->_motorDirPin2);
  Serial.println(" (dri2)");

  // sets the pins as outputs:
  pinMode(this->_motorDirPin1, OUTPUT);
  pinMode(this->_motorDirPin2, OUTPUT);
  pinMode(this->_motorPwmPin, OUTPUT);

  // make sure the motor is stopped
  digitalWrite(this->_motorDirPin1, LOW);
  digitalWrite(this->_motorDirPin2, LOW);

  // start pwm signal
  analogWrite(this->_motorPwmPin, DUTY_CYCLE);

  this->_running = false;

  user_cb = cb;
}

void Solscreen::setDuration(uint16_t duration){
  while(this->_running);
  this->_duration = duration;
}

void Solscreen::goUp(void){
  if(!this->_running && (this->_status != CURTAIN_UP)){
    digitalWrite(this->_motorDirPin1, HIGH);
    os_timer_arm(&runTimer, this->_duration, false);
    this->_running = true;
    this->_status = GOING_UP;
  }
}

void Solscreen::goDown(void){
  if(!this->_running && (this->_status != CURTAIN_DOWN)){
    digitalWrite(this->_motorDirPin2, HIGH);
    os_timer_arm(&runTimer, this->_duration, false);
    this->_running = true;
    this->_status = GOING_DOWN;
  }
}

void Solscreen::stop(void){
  digitalWrite(this->_motorDirPin1, LOW);
  digitalWrite(this->_motorDirPin2, LOW);
  this->_running = false;
  switch(this->_status){
    case GOING_UP: this->_status = CURTAIN_UP; break;
    case GOING_DOWN: this->_status = CURTAIN_DOWN; break;
    default: this->_status = STATE_ERROR;
  }
}

ScreenStatus_t Solscreen::getStatus(void){
  return this->_status;
}

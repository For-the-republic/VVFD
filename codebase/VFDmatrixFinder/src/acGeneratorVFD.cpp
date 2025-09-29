#include "Arduino.h"
#include "acGeneratorVFD.h"
#include "wire.h"

acGeneratorVFD::acGeneratorVFD()
{
  _state = 0; // start in off state
}
void acGeneratorVFD::begin(uint8_t acPin1, bool state)
{
  // """if the state is 0, the ac generator is in DC mode, if 1 it is in AC mode"""
  _acPin1 = acPin1;
  _state = state;
  pinMode(_acPin1, OUTPUT);
  if (_state == false)
  {
    analogWrite(_acPin1, 127);
  }
  else
  {
    analogWrite(_acPin1, 0);
  }
}


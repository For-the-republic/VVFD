#include "Arduino.h"
#include "acGeneratorVFD.h"
#include "wire.h"

acGeneratorVFD::acGeneratorVFD()
{
  _state = 0; // start in off state
}
void acGeneratorVFD::begin(uint8_t acPin1, uint8_t acPin2, bool state)
{
  // """if the state is 0, the ac generator is in DC mode, if 1 it is in AC mode"""
  _acPin1 = acPin1;
  _acPin2 = acPin2;
  _state = state;
  pinMode(_acPin1, OUTPUT);
  pinMode(_acPin2, OUTPUT);
  if (_state == false)
  {
    digitalWrite(_acPin1, HIGH);
    digitalWrite(_acPin2, LOW);
  }
  else


{
  
    TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(COM0B0) | _BV(WGM11);
    TCCR0B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
    ICR1 = 159;
    OCR0A = 0xA0 >> 1;
    OCR0B = 0xA0 >> 1;

}
}


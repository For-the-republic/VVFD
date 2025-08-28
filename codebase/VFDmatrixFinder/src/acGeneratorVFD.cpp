#include "Arduino.h"
#include "acGeneratorVFD.h"
#include "wire.h"

acGeneratorVFD::acGeneratorVFD() {
    _state = 0; //start in off state
}
void acGeneratorVFD::begin(uint8_t acPin1, uint8_t acPin2, bool state) {
    // """if the state is 0, the ac generator is in DC mode, if 1 it is in AC mode"""
    _acPin1 = acPin1;
    _acPin2 = acPin2;
    _state = state;
    pinMode(_acPin1, OUTPUT);
    pinMode(_acPin2, OUTPUT);
    if(_state==0) {
        digitalWrite(_acPin1,HIGH);
        digitalWrite(_acPin2,LOW);
    } else {
          TCCR1A = 0; //clear timer registers
            TCCR1B = 0;
            TCNT1 = 0;

            //ICR1 and Prescaler sets frequency
            //no prescaler .0625 us per count @ 16Mh
            //80 counts x .0625 = 5 us = 200Khz

            TCCR1B |= _BV(CS10); //no prescaler
            ICR1 = 40;//PWM mode counts up and back down for 80 counts

            OCR1A = 21; //Pin 9 match
            //output A set rising/clear falling
            //Rise at TCNT 21 upslope, High 38 counts, Fall at TCNT 21 downslope
            //47,5% Duty Cycle Pulse centered on TCNT 40. High 38 Low 42
            TCCR1A |= _BV(COM1A1) | _BV(COM1A0); //output A set rising/clear falling

            OCR1B = 19; //Pin 10 match
            //output B clear rising/set falling
            //Fall at TCNT 19 upslope, Low 42, Rise at TCNT 19 downslope
            //47.5% Duty Cycle Pulse centered on TCNT 0. High 38 Low 42
            TCCR1A |= _BV(COM1B1); //output B clear rising/set falling

            TCCR1B |= _BV(WGM13); //PWM mode with ICR1 Mode 10
            TCCR1A |= _BV(WGM11); //WGM13:WGM10 set 1010
    }
}
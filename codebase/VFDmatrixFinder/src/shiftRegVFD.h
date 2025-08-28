#ifndef shiftRegVFD_h
#define shiftRegVFD_h
#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
//where you output the list to the shift register

// """TODO:
// - add a function to automattically convert grids into the right format
// - remember to serial print
// """


class shiftRegVFD {
    public:

    shiftRegVFD();
    void begin(uint8_t dataPin, uint8_t strobePin, uint8_t clockPin);
    void outputList(uint8_t list[]);
    uint8_t convertToGrid( uint8_t list[]);

    
    private:
    uint8_t _dataPin;
    uint8_t _strobePin;
    uint8_t _clockPin;

};
#endif

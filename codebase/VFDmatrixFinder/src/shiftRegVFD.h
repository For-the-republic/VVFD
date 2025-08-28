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
    void begin(uint8_t dataPin, uint8_t strobePin, uint8_t clockPin, size_t NO_OF_PINS);
    void outputList(uint8_t list[]);
    void setMatrix(uint8_t *matrix, size_t row, size_t NO_OF_PINS);
    void updateMatrix(uint8_t matrix[], size_t row);
    void setDisplay(uint8_t matrix[], size_t row);
    void outputMatrix(int row);
    uint8_t convertToGrid( uint8_t list[]);

    
    private:
    uint8_t _dataPin;
    uint8_t _strobePin;
    uint8_t _clockPin;
    uint8_t _internalMatrix[0][0];

};
#endif

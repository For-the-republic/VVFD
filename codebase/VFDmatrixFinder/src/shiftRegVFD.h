#ifndef shiftRegVFD_h
#define shiftRegVFD_h
#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <stdint.h>
//where you output the list to the shift register

// """TODO:
// - add a function to automattically convert grids into the right format
// - remember to serial print
// """


class shiftRegVFD {
    public:

    shiftRegVFD();
    ~shiftRegVFD();
    void begin(uint8_t dataPin, uint8_t strobePin, uint8_t clockPin, size_t NO_OF_PINS);
        // directly outputs the information to the shift register, works with standard formatting
    void outputList(uint8_t* matrix,size_t length);
        //sets up an internal matrix to simplify code in main
    void setMatrix(uint64_t *matrix, size_t row );
        // updates a single row of the internal matrix
    void updateMatrix(uint64_t matrix, size_t row);
     // sets the internal matrix to the new matrix delinking the input from the output so that you can pass in an image without it ovveriding the segment/grid pins
    void setDisplay(uint64_t matrix, size_t row);
      // outputs the internal matrix to the shift register OF THE GIVEN GRID
    void outputMatrix(uint64_t row);

    uint8_t returnRow( size_t row);
    uint8_t convertToGrid( uint64_t list[]);

    
    private:
    uint8_t _dataPin;
    uint8_t _strobePin;
    uint8_t _clockPin;

    size_t _NO_OF_PINS;
    size_t _NO_OF_GRIDS;
    uint64_t* _internalMatrix;  
};
#endif

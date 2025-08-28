#include "Arduino.h"
#include "shiftRegVFD.h"
#include <Wire.h>
#include <SPI.h>


shiftRegVFD::shiftRegVFD() {
    //constructor
    _dataPin = 0;
    _strobePin = 0;
    _clockPin = 0;
}
void shiftRegVFD::begin(uint8_t dataPin, uint8_t strobePin, uint8_t clockPin) {
    _dataPin = dataPin;
    _strobePin = strobePin;
    _clockPin = clockPin;
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    pinMode(_strobePin, OUTPUT);
}
void shiftRegVFD::outputList(uint8_t data[]) {
     uint8_t temp[sizeof(data)];
    for(int i=0; i<sizeof(data); i++) {
        //checks if the value is 1, OR if its a currently selected grid. else set to zero
        if(data[i]==1 || data[i]==2) {

        temp[i] = 1;
    } else {
        temp[i] =0;
    }
}
    SPI.transfer(temp, sizeof(temp));
    digitalWrite(_strobePin, HIGH);
    digitalWrite(_strobePin, LOW);
}
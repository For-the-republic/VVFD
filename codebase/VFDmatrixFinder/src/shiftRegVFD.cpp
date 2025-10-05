#include "Arduino.h"
#include "shiftRegVFD.h"
#include <Wire.h>
#include <SPI.h>

shiftRegVFD::shiftRegVFD()
{
    // constructor
    _dataPin = 0;
    _strobePin = 0;
    _clockPin = 0;
    _NO_OF_PINS = 0;
    _NO_OF_GRIDS = 0;
    _internalMatrix = nullptr;
}
shiftRegVFD::~shiftRegVFD()
{
    if (_internalMatrix)
    {
        delete[] _internalMatrix;
        _internalMatrix = nullptr;
    }
}
void shiftRegVFD::begin(uint8_t dataPin, uint8_t strobePin, uint8_t clockPin, size_t pinCount)
{
    _dataPin = dataPin;
    _strobePin = strobePin;
    _clockPin = clockPin;
    _NO_OF_PINS = pinCount;
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_strobePin, OUTPUT);

    SPI.begin();
    SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));

}
void shiftRegVFD::outputList(uint8_t *data, size_t length)
{
    // directly outputs the information to the shift register, works with standard formatting
     // Break rowData into bytes (LSB first)
    digitalWrite(_strobePin, LOW); // Begin transmission
    for(size_t i =0; i<length; i++){
        SPI.transfer(data[i]);
    }
    delayMicroseconds(175);
    digitalWrite(_strobePin, HIGH); // End transmission
    
    // Now send buffer over SPI

}


void shiftRegVFD::setMatrix(uint64_t *pointer, size_t rows)
{
    // Free old
    if (_internalMatrix)
    {
        delete[] _internalMatrix;
    }

    _NO_OF_GRIDS = rows;

    // Allocate new
    _internalMatrix = new uint64_t[rows];

    // Copy values
    for (size_t i = 0; i < rows; i++)
    {
        _internalMatrix[i] = pointer[i];
    }
}

void shiftRegVFD::updateMatrix(uint64_t matrix, size_t row)
{
    if (row >= _NO_OF_GRIDS)
        return; // prevent out-of-bounds

    _internalMatrix[row] = matrix;
}

// --- setDisplay: overwrite one row safely ---
void shiftRegVFD::setDisplay(uint64_t value, size_t row)
{
    if (row >= _NO_OF_GRIDS) return;

    _internalMatrix[row] = value;
}
void shiftRegVFD::outputMatrix(uint64_t row)
{
    if (row >= _NO_OF_GRIDS) return;

    uint64_t rowData = _internalMatrix[row];

    // Break rowData into bytes (LSB first)
    size_t numBytes = (_NO_OF_PINS) / 8;
    uint8_t buffer[numBytes];

    for (size_t i = 0; i < numBytes; i++)
    {
        buffer[i] = (rowData >> (i * 8)) & 0xFF;
    }

    // Now send buffer over SPI
    outputList(buffer, numBytes);
}
uint8_t shiftRegVFD::returnRow( size_t row){
    if (row >= _NO_OF_GRIDS) return 0;

    uint64_t rowData = _internalMatrix[row];
    return (uint8_t)(rowData & 0xFF);
}
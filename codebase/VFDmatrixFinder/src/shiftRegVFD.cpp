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
}
void shiftRegVFD::outputList(uint8_t* data, size_t length)
{
    // directly outputs the information to the shift register, works with standard formatting
    uint8_t* temp = new uint8_t[length];
    for (size_t i = 0; i < length; i++)
    {
        // checks if the value is 1, OR if its a currently selected grid. else set to zero
        if ((data[i]) == 1 || data[i] == 2)
        {
            temp[i] = 1;
        }
        else
        {
            temp[i] = 0;
        }
    }
        size_t numBytes = length / 8; // the length will always be equal to the number of shift registers
    uint8_t buffer[numBytes];            // Packed bytes

    // Initialize buffer
    for (size_t i = 0; i < numBytes; i++) {
        buffer[i] = 0;
    }

    // Pack bits into buffer
    for (size_t i = 0; i < length; i++) {
        size_t byteIndex = i / 8;
        size_t bitIndex = 7 - (i % 8); // MSB first
        if (temp[i]) {
            buffer[byteIndex] |= (1 << bitIndex);
        }
    }
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    for (size_t i = 0; i < numBytes; i++) {

        SPI.transfer(buffer[i]);
    }
    digitalWrite(_strobePin, HIGH);
    delay(1);
    digitalWrite(_strobePin, LOW);
    
    SPI.endTransaction();
    delete[] temp;
}

void shiftRegVFD::setMatrix(uint8_t *pointer, size_t row, size_t col)
{
    _NO_OF_GRIDS = row;
    _NO_OF_PINS  = col;

    // Free old matrix if needed
    if (_internalMatrix) {
        for (size_t i = 0; i < _NO_OF_GRIDS; i++) {
            delete[] _internalMatrix[i];
        }
        delete[] _internalMatrix;
    }

    // Allocate new matrix
    _internalMatrix = new uint8_t*[row];
    for (size_t i = 0; i < row; i++) {
        _internalMatrix[i] = new uint8_t[col];
    }

    // Fill values from pointer
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            _internalMatrix[i][j] = pointer[i * col + j];
        }
    }
    
}
void shiftRegVFD::updateMatrix(uint8_t *matrix, size_t row)
{
    if (row >= _NO_OF_GRIDS) return; // prevent out-of-bounds

    for (size_t i = 0; i < _NO_OF_PINS; i++)
    {
        // Only update displayable elements (<2)
        if (_internalMatrix[row][i] < 2)
        {
            _internalMatrix[row][i] = matrix[i];
        }
    }
}

// --- setDisplay: overwrite one row safely ---
void shiftRegVFD::setDisplay(uint8_t *matrix, size_t row)
{
    if (row >= _NO_OF_GRIDS) return; // prevent out-of-bounds

    for (size_t i = 0; i < _NO_OF_PINS; i++)
    {
        // Only update displayable elements (<2)
        if (_internalMatrix[row][i] < 2)
        {
            _internalMatrix[row][i] = matrix[i];
        }
    }
}
void shiftRegVFD::outputMatrix(uint8_t row)    // outputs the internal matrix to the shift register

{
    outputList((_internalMatrix[row]), _NO_OF_PINS);
}
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
    _internalMatrix[0][0] = {0};
}
void shiftRegVFD::begin(uint8_t dataPin, uint8_t strobePin, uint8_t clockPin, size_t NO_OF_PINS)
{
    _dataPin = dataPin;
    _strobePin = strobePin;
    _clockPin = clockPin;
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    pinMode(_strobePin, OUTPUT);
}
void shiftRegVFD::outputList(uint8_t data[])
{
    // directly outputs the information to the shift register, works with standard formatting
    uint8_t temp[sizeof(data)] = {0};
    for (int i = 0; i < sizeof(data); i++)
    {
        // checks if the value is 1, OR if its a currently selected grid. else set to zero
        if (data[i] == 1 || data[i] == 2)
        {

            temp[i] = 1;
        }
        else
        {
            temp[i] = 0;
        }
    }
    SPI.transfer(temp, sizeof(temp));
    digitalWrite(_strobePin, HIGH);
    digitalWrite(_strobePin, LOW);
}
void shiftRegVFD::setMatrix(uint8_t *pointer, size_t row, size_t col)
{
    //sets up an internal matrix to simplify code in main
    _internalMatrix[row][col] = {0};
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            _internalMatrix[i][j] = pointer[i * col + j];
        }
    }

}
void shiftRegVFD::updateMatrix(uint8_t matrix[], size_t row)
{
    // updates a single row of the internal matrix
    for (int i = 0; i < sizeof(_internalMatrix[row]); i++)
    {
        // checks if the value is 1, OR if its a currently selected grid. else set to zero
        _internalMatrix[row][i] = matrix[i];
    }
}
void shiftRegVFD::setDisplay(uint8_t matrix[], size_t row)
{
    // sets the internal matrix to the new matrix
    //delinking the input from the output so that you can pass in an image without it ovveriding the segment/grid pins
    size_t i = 0;
    size_t j = 0;
    while (i < sizeof(_internalMatrix[row]))
    {
        // checks if the value is 1, OR if its a currently selected grid. else set to zero
        if (_internalMatrix[row][i] < 2)
        {
            _internalMatrix[row][i] = matrix[j];
            j++;
            i++;
        }
        else
        {
            i++;
        }
    }
}
void shiftRegVFD::outputMatrix(int row)
{
    // outputs the internal matrix to the shift register
    outputList(_internalMatrix[row]);
}
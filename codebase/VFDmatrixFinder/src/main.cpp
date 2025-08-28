#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "acGeneratorVFD.h"
#include "shiftRegVFD.h"
#include "ammeterVFD.h"
#include "stdint.h"

bool AC_OR_DC = true;     // true = AC, false = DC
const int NO_OF_PINS = 8; // set this to the number of pins from the top right down

// base values for the grid/segments
float BASE_SEGMENT = NULL;
float BASE_GRID = NULL;
long DELAY = 500; // delay time between updates

// globals for the total numbers found
size_t noOfGrids = 0;
size_t noOfSegments = 0;
// the various lists used in making the thing
uint8_t outputList[NO_OF_PINS] = {0};
uint8_t finalList[][NO_OF_PINS] = {0};
// 0/1 = binary output, 2 = grid, 3 = no connection

// timings for the custom bits
long currentMillis;
unsigned long ammeterTiming;
unsigned long gridTiming;
// the custom parts
AmmeterVFD ammeter;
shiftRegVFD shiftReg;
acGeneratorVFD acGen;

// put function declarations here:
bool timing(unsigned long &timing, long &DELAY)
{
  // if the Led is off, we must wait for the interval to expire before turning it on
  if ((currentMillis - timing) >= (DELAY))
  {
    timing += (DELAY);
    return true;
  }
  return false;
}
void convertMatrix()
{
  // converts a single array into a matrix of all the grid combos
  for (size_t i = 0; i < noOfGrids; i++)
  {
    bool temp = false;
    for (size_t j = 0; j < NO_OF_PINS; j++)
    {
      if (temp == false && outputList[j] == 2)
      {
        temp = true;
        finalList[i][j] = 2;
      }
      else if (temp == true && outputList[j] == 2)
      {
        finalList[i][j] = 3;
      }
    }
  }
}

void findGrids()
{
  // the first step of the process, finds all valid grids so the segments can be found
  uint8_t gridList[NO_OF_PINS] = {0};
  gridList[0] = 1;
  size_t i = 1;
  float reading = 0;
  shiftReg.outputList(gridList);
  ammeter.toLCS();
  // loops over all pins
  while (true)
  {
    // checks if enough time has passed to stabilise the reading
    currentMillis = millis();
    if (timing(gridTiming, DELAY))
    {
      // if it has, make sure i has not gone oveer
      if (i < NO_OF_PINS)
      {
        reading = ammeter.readCurrent();
        // if the reading is not 0, then it is a grid
        if (reading > 0)
        {
          outputList[i] = 2;
          BASE_GRID += reading;
          noOfGrids++;
          Serial.print("found grid at pin " + String(i) + " , reading = " + String(reading) + "\n");
        }
        // move onto the next pin
        i++;
        gridList[i] = 1;
        gridList[i - 1] = 0;
        shiftReg.outputList(gridList);
      }
      else
      {
        break;
      }
    }
  }
  BASE_GRID = BASE_GRID / noOfGrids;
}
void findSegments()
{

  // first
  for (size_t i; i < NO_OF_PINS; i++)
  {
    size_t j = 0;
    while (j < NO_OF_PINS)
    {
      // checks to make sure theres no overwrite of pins sice we dont know if its a grid or not
      if (finalList[i][j] != 0 && finalList[i][j] != 1)
      {
        j++;
      }
      else
      {

        finalList[i][j] = 1;
        shiftReg.outputList(finalList[i]);
      }
      // then start reading the ammeter to see if theres a connection
      currentMillis = millis();
      if (timing(gridTiming, DELAY))
      {
        float reading = ammeter.readCurrent();

        if (reading == BASE_GRID)
        {
          finalList[i][j] = 3; // no connection here
        }
        else
        {
          finalList[i][j] = 0; // is a connection, set to zero and find the next pin

          BASE_SEGMENT += reading - BASE_GRID;
          noOfSegments++;
          Serial.print("found segment at grid " + String(i) + " pin " + String(j) + " , reading = " + String(reading) + "\n");
          j++;
        }
      }
    }
  }
  BASE_SEGMENT = BASE_SEGMENT / noOfSegments;
}

void displayMatrix()
{
  // almost there, now showing that all pin combos the system has found
  size_t i = 0;
  size_t j = 0;

  while (true)
  {
    // checks to make sure theres no overwrite of pins sice we dont know if its a grid or not

    if (finalList[i][j] != 0 && finalList[i][j] != 1)
    {
      if (j >= NO_OF_PINS - 1)
      {
        j = 0;
        i++;
      }
      j++;
    }
    else
      finalList[i][j] = 1;
    shiftReg.outputList(finalList[i]);
    // then start reading the ammeter to see if theres a connection
    currentMillis = millis();
    if (timing(gridTiming, DELAY))
    {
      finalList[i][j] = 0;
      j++;
    }
  }
}

void setup()
{
  currentMillis = 0;
  ammeter.begin(A0, A1, A2, A3);
  ammeter.toHCS();
  shiftReg.begin(12, 11, 13);
  acGen.begin(5, 6, AC_OR_DC);
  Serial.begin(9600);
  Serial.print("setup complete, going into base condition mode");
}

void loop()
{

  findGrids();
  finalList[noOfGrids][NO_OF_PINS] = {0};
  convertMatrix();
  findSegments();
  displayMatrix();
}

// put function definitions here:

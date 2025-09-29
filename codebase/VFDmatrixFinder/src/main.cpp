#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "acGeneratorVFD.h"
#include "shiftregVFD.h"
#include "ammeterVFD.h"
#include "stdint.h"

#define VFD_SIZE 7
//base pin globals
bool AC_OR_DC = true; 
const unsigned uint8_t NO_OF_PINS = 8*VFD_SIZE; // set this to the number of pins from the top right down
#define DATA_PIN = 12;   // pin connected to the data pin of the shift register
#define STROBE_PIN = 11; // pin connected to the strobe pin of the shift register
#define CLOCK_PIN = 13;  // pin connected to the clock pin of the shift register
const uint8_t HCSIN = A0;
const uint8_t HCSEN = A1;
const uint8_t LCSIN = A2;
const uint8_t LCSEN = A3;
//these two cannot be changed, if you do you have to modify the acGeneratorVFD library
const unsigned uint8_t ACPINONE = 6; // pin connected to the ac generator one side


// base values for the grid/segments
static float BASE_SEGMENT = NULL;
static float BASE_GRID = NULL;
static long DELAY = 500; // delay time between updates

// globals for the total numbers found
static size_t noOfGrids = 0;
static size_t noOfSegments = 0;
// the various lists used in making the thing
static uint8_t OutputList[NO_OF_PINS] = {0};
static uint8_t finalList[][NO_OF_PINS] = {0};
// 0/1 = binary output, 2 = grid, 3 = no connection

// timings for the custom bits
static long currentMillis;
static unsigned long ammeterTiming;
static unsigned long gridTiming;
// the custom parts
AmmeterVFD ammeter;
shiftRegVFD shiftReg;
acGeneratorVFD acGen;

// put function declarations here:le
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
      if (temp == false && OutputList[j] == 2)
      {
        temp = true;
        finalList[i][j] = 2;
      }
      else if (temp == true && OutputList[j] == 2)
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
          OutputList[i] = 2;
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

    if (finalList[i][j] != 0 && finalList[i][j] != 1 && timing(gridTiming, DELAY))
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
  acGen.begin(ACPINONE, AC_OR_DC);
  ammeter.begin(HCSIN, HCSEN, LCSIN, LCSEN);
  ammeter.toHCS();
  shiftReg.begin(DATA_PIN, STROBE_PIN, CLOCK_PIN, NO_OF_PINS);
  Serial.begin(9600);
  Serial.print("setup complete, going into base condition mode");
}

void loop()
{

  findGrids();
  finalList[noOfGrids][NO_OF_PINS] = {0};
  convertMatrix();
  findSegments();
  for(size_t i = 0; i < noOfGrids; i++){
    for(size_t j = 0; j < NO_OF_PINS; j++) {
      Serial.print(finalList[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  displayMatrix();
}

// put function definitions here:

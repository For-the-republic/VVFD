// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include "shiftregVFD.h"
// #include "ammeterVFD.h"
// #include <stdint.h>

// const uint8_t  VFD_SIZE =7;
// const uint8_t  DATA_PIN =12;  // pin connected to the data pin of the shift register
// const uint8_t  STROBE_PIN =11; // pin connected to the strobe pin of the shift register
// const uint8_t  CLOCK_PIN =13;  // pin connected to the clock pin of the shift register
// //base pin globals
// bool AC_OR_DC = true; 
// const uint8_t NO_OF_PINS = 8*VFD_SIZE; // set this to the number of pins from the top right down
// const uint8_t HCSIN = A0;
// const uint8_t HCSEN = A1;
// const uint8_t LCSIN = A2;
// const uint8_t LCSEN = A3;
// //these two cannot be changed, if you do you have to modify the acGeneratorVFD library


// // base values for the grid/segments
// static float BASE_SEGMENT = NULL;
// static float BASE_GRID = NULL;
// static long DELAY = 500; // delay time between updates

// // globals for the total numbers found
// static size_t noOfGrids = 0;
// static size_t noOfSegments = 0;
// // the various lists used in making the thing
// static uint8_t OutputList[NO_OF_PINS] = {0};
// static uint8_t finalList[][NO_OF_PINS] = {0};
// // 0/1 = binary output, 2 = grid, 3 = no connection

// // timings for the custom bits
// static long currentMillis;
// static unsigned long ammeterTiming;
// static unsigned long gridTiming;
// // the custom parts
// AmmeterVFD ammeter;
// shiftRegVFD shiftReg;

// // put function declarations here:le
// bool timing(unsigned long &timing, long &DELAY)
// {
//   // if the Led is off, we must wait for the interval to expire before turning it on
//   if ((currentMillis - timing) >= (DELAY))
//   {
//     DELAY += (timing);
//     return true;
//   }
//   return false;
// }
// void convertMatrix()
// {
//   // converts a single array into a matrix of all the grid combos
//   for (size_t i = 0; i < noOfGrids; i++)
//   {
//     bool temp = false;
//     for (size_t j = 0; j < NO_OF_PINS; j++)
//     {
//       if (temp == false && OutputList[j] == 2)
//       {
//         temp = true;
//         finalList[i][j] = 2;
//       }
//       else if (temp == true && OutputList[j] == 2)
//       {
//         finalList[i][j] = 3;
//       }
//     }
//   }
// }

// void findGrids()
// {
//   // the first step of the process, finds all valid grids so the segments can be found
//   uint8_t gridList[NO_OF_PINS] = {0};
//   gridList[0] = 1;
//   size_t i = 1;
//   float reading = 0;
//   shiftReg.outputList(gridList);
//   ammeter.toLCS();
//   // loops over all pins
//   while (true)
//   {
//     // checks if enough time has passed to stabilise the reading
//     currentMillis = millis();
//     if (timing(gridTiming, DELAY))
//     {
//       // if it has, make sure i has not gone oveer
//       if (i < NO_OF_PINS)
//       {
//         reading = ammeter.readCurrent();
//         // if the reading is not 0, then it is a grid
//         if (reading > 0)
//         {
//           OutputList[i] = 2;
//           BASE_GRID += reading;
//           noOfGrids++;
//           Serial.print("found grid at pin " + String(i) + " , reading = " + String(reading) + "\n");
//         }
//         // move onto the next pin
//         i++;
//         gridList[i] = 1;
//         gridList[i - 1] = 0;
//         shiftReg.outputList(gridList);
//       }
//       else
//       {
//         break;
//       }
//     }
//   }
//   BASE_GRID = BASE_GRID / noOfGrids;
// }
// void findSegments()
// {

//   // first
//   for (size_t i; i < NO_OF_PINS; i++)
//   {
//     size_t j = 0;
//     while (j < NO_OF_PINS)
//     {
//       // checks to make sure theres no overwrite of pins sice we dont know if its a grid or not
//       if (finalList[i][j] != 0 && finalList[i][j] != 1)
//       {
//         j++;
//       }
//       else
//       {

//         finalList[i][j] = 1;
//         shiftReg.outputList(finalList[i]);
//       }
//       // then start reading the ammeter to see if theres a connection
//       currentMillis = millis();
//       if (timing(gridTiming, DELAY))
//       {
//         float reading = ammeter.readCurrent();

//         if (reading == BASE_GRID)
//         {
//           finalList[i][j] = 3; // no connection here
//         }
//         else
//         {
//           finalList[i][j] = 0; // is a connection, set to zero and find the next pin

//           BASE_SEGMENT += reading - BASE_GRID;
//           noOfSegments++;
//           Serial.print("found segment at grid " + String(i) + " pin " + String(j) + " , reading = " + String(reading) + "\n");
//           j++;
//         }
//       }
//     }
//   }
//   BASE_SEGMENT = BASE_SEGMENT / noOfSegments;
// }

// void displayMatrix()
// {
//   // almost there, now showing that all pin combos the system has found
//   size_t i = 0;
//   size_t j = 0;

//   while (true)
//   {
//     // checks to make sure theres no overwrite of pins sice we dont know if its a grid or not

//     if (finalList[i][j] != 0 && finalList[i][j] != 1 && timing(gridTiming, DELAY))
//     {
//       if (j >= NO_OF_PINS - 1)
//       {
//         j = 0;
//         i++;
//       }
//       j++;
//     }
//     else
//       finalList[i][j] = 1;
//     shiftReg.outputList(finalList[i]);
//     // then start reading the ammeter to see if theres a connection
//     currentMillis = millis();
//     if (timing(gridTiming, DELAY))
//     {
//       finalList[i][j] = 0;
//       j++;
//     }
//   }
// }

// void setup()
// {
//   currentMillis = 0;
//   ammeter.begin(HCSIN, HCSEN, LCSIN, LCSEN);
//   ammeter.toHCS();
//   shiftReg.begin(DATA_PIN, STROBE_PIN, CLOCK_PIN, NO_OF_PINS);
//   Serial.begin(9600);
//   Serial.print("setup complete, going into base condition mode");
// }

// void loop()
// {

//   findGrids();
//   finalList[noOfGrids][NO_OF_PINS] = {0};
//   convertMatrix();
//   findSegments();
//   for(size_t i = 0; i < noOfGrids; i++){
//     for(size_t j = 0; j < NO_OF_PINS; j++) {
//       Serial.print(finalList[i][j]);
//       Serial.print(" ");
//     }
//     Serial.println();
//   }
//   displayMatrix();
// }

// put function definitions here:



#include <Arduino.h>
#include <shiftRegVFD.h>
#include <Wire.h>
#include <stdint.h>
#include <string.h>
#include <SPI.h>
#include "ammeterVFD.h"
const uint8_t  DATA_PIN =11;  // pin connected to the data pin of the shift register
const uint8_t  STROBE_PIN =10; // pin connected to the strobe pin of the shift register
const uint8_t  CLK_PIN =13;  // pin connected to the clock pin of the shift register
const int NO_OF_GIDS = 3;
const int NO_OF_SEGS = 48;
long DELAY = 500;
long currentMillis = 0;
//this sets the locations of which grids you are updating
const int GRID1 = 0;
const int GRID2 = 1;
const int GRID3 = 2;
  static bool firstRun = true;

int k = 0;
int j = 0;
uint8_t image[128] = {0};
String readString = "";
AmmeterVFD ammeter = AmmeterVFD();

shiftRegVFD display = shiftRegVFD();
uint8_t thing[NO_OF_SEGS] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 0, 0, 0, 0, 3, 2, 3, 4, 4, 4, 4, 4};
uint8_t thing2 [NO_OF_SEGS] = 
{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
 3, 3,3 , 3, 3, 2, 3, 3, 4, 4, 4, 4};

static uint8_t matrix[NO_OF_GIDS][NO_OF_SEGS] = {

{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
 3, 3,3 , 3, 3, 2, 3, 3, 4, 4, 4, 4},
 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 3, 3, 2, 4, 4, 4, 4, 4},

{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 0, 0, 0, 0, 2, 3, 3, 4, 4, 4, 4, 4}

  };
bool timing(unsigned long DELAY) {
  static unsigned long lastTime = 0;  
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= DELAY) {
    lastTime = currentTime;
    return true;  // Interval has passed
  }
  return false;  } // Interval not yet reached
void displayImage(uint8_t* image)
{
    uint8_t display1[NO_OF_SEGS] = {0};
    uint8_t display2[NO_OF_SEGS] = {0};
    uint8_t display3[NO_OF_SEGS] = {0};
      // since the display data comes in going down, we need to split it to go across
    for(size_t i =0; i<16; i++){
      for(size_t j =0, k=(i*8); j<56 && k<(16+8*i); j+=8, k++){
          display1[j+i] = image[k];
          display2[j+i] = image[k+40];
          display3[j+i] = image[k+80];
       }
      }
            //converts the bad apple image into the VFD matrix format
    display.updateMatrix(display1, GRID1);
    display.updateMatrix(display2, GRID2);
    display.updateMatrix(display3, GRID3);
    display.outputMatrix(0);
    delay(3.32/3);
    display.outputMatrix(1);
    delay(3.32/3);
    display.outputMatrix(2);
    delay(3.32/3);
  
}
void convertStringToU64(String str)
{
  for (unsigned int i = 0; i < str.length(); i++)
  {
    if(str[i] =='0'){
      image[i] = 0;
    } else {
      image[i] = 1;
    }
  };
}

void setup()
{
  Serial.begin(9600);
  display.begin(DATA_PIN, STROBE_PIN, CLK_PIN, NO_OF_SEGS);
  ammeter.begin(A0, A1, A2, A3);
  ammeter.toHCS();

  display.setMatrix(*matrix, NO_OF_GIDS,NO_OF_SEGS);
  Serial.print("setup complete, going into base condition mode");
  // display.updateMatrix(thing,0);


}

void loop()
{

  display.outputMatrix(0);


  display.outputMatrix(1);


  display.outputMatrix(2);
  if (timing(DELAY)) {
    if (firstRun) {
      display.updateMatrix(thing2 ,0);
      firstRun = false;
          Serial.println(firstRun);

    }else {
      display.updateMatrix(thing ,0);
      firstRun = true;
    Serial.println(firstRun);

      }
  }


  // while (Serial.available())
  // {
  //   char c = Serial.read();
  //   Serial.print(c);
  //   if (c != '\n')
  //     readString += c;
  //   else
  //   {
  //     Serial.print(readString + "|");
  //     convertStringToU64(readString);
  //     displayImage((image));
  //     readString = "";
  //   }

  // }

}
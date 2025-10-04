
#include <Arduino.h>
#include <shiftRegVFD.h>
#include <Wire.h>
#include <stdint.h>
#include <string.h>
#include <SPI.h>
#include "ammeterVFD.h"

const uint8_t DATA_PIN = 11;   // pin connected to the data pin of the shift register
const uint8_t STROBE_PIN = 10; // pin connected to the strobe pin of the shift register
const uint8_t CLK_PIN = 13;    // pin connected to the clock pin of the shift register
const int NO_OF_GIDS = 3;
const int NO_OF_SEGS = 56;
const long DELAY = 500;
// this sets the locations of which grids you are updating
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
String frame = "10001100011111110001100011000100000";
uint64_t thing[NO_OF_SEGS] =
    {
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 2, 3, 3,
        3, 3, 3, 3, 3,
        3, 3, 3, 3, 3,
        3, 3, 3, 3, 4,
        4};


static uint64_t matrix[NO_OF_GIDS] = {
    //    Digiets/Segments                           | Grids    |extras
    // 0b1000110001111111000110001100010000000000000000000100000000000000,
    0b1000000010000000100000001000000010000000100000001000000010000000,

    0b0111000100001000010000100011100000000000000000000000000000000000,
    0b1111111111111111111111111111111111111111000011100000001000000000};
bool timing(unsigned long DELAY)
{
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= DELAY)
  {
    lastTime = currentTime;
    return true; // Interval has passed
  }
  return false;
} // Interval not yet reached

void frameToDisplays(uint32_t* frame)
{

  // Clear old data
  uint64_t displayData[NO_OF_GIDS];
  for (int d = 0; d < NO_OF_GIDS; d++)
  {
    displayData[d] = 0;
  }

  displayData[0] = (((uint64_t)(frame[0] & 0x7C000000) << 33) | 
                   ((uint64_t)(frame[0] & 0x0000F800) << 43) |
                   ((uint64_t)(frame[1] & 0x7C000000) <<18) |
                   ((uint64_t)(frame[1] & 0x00007C00) << 38) |
                   ((uint64_t)(frame[2] & 0x7C000000) << 13) |
                   ((uint64_t)(frame[2] & 0x00007C00) << 23) |
                   ((uint64_t)(frame[3] & 0x003E0000) << 3)
);
  displayData[1] = (((uint64_t)(frame[0] & 0x07C00000)<<43) |
                   ((uint64_t)(frame[0] & 0x000007C0) <<53) |
                  ((uint64_t)(frame[1] & 0x03E00000) << 28) |
                  ((uint64_t)(frame[1] & 0x000003E0) << 42) |
                  ((uint64_t)(frame[2] & 0x7C000000) << 18) |
                  ((uint64_t)(frame[2] & 0x00007C00) << 32) |
                  ((uint64_t)(frame[3] & 0x0003E000) << 8)
                  );
  displayData[2] = (((uint64_t)(frame[0] & 0x003E0000)<<43) |
                   ((uint64_t)(frame[0] & 0x0000003F) << 54) |
                   ((uint64_t)(frame[1] & 0x001F0000) << 33) |
                   ((uint64_t)(frame[2] & 0x0000001F) <<44) |
                  ((uint64_t)(frame[2] & 0x03E00000) << 23) |
                  ((uint64_t)(frame[3] & 0x0000F800) << 34)
                  );
  // bit masks over the grids to stop too much current being drawn
  // displayData[0] = (displayData[0]<<15 & 0b1111111111111111111111111111111111111111000011100000000000000000);
  // displayData[1] = (displayData[1]<<15 & 0b1111111111111111111111111111111111111111000011100000000000000000);
  // displayData[2] = (displayData[2]<<15 & 0b1111111111111111111111111111111111111111000011100000000000000000);
  // // now mask back over
  // char thingy = "0000000000000000000000000000"
  displayData[0] = (displayData[0] | 0b0000000000000000000000000000000000000000000000000100000000000000);
  displayData[1] = (displayData[1] | 0b0000000000000000000000000000000000000000000000000010000000000000);
  displayData[2] = (displayData[2] | 0b0000000000000000000000000000000000000000000000000000001000000000);
  // Serial.print("\n");
  // Serial.print("1111100000111111111110000111111111111000011111111111000001111111100000000000000000000000000000000000000000000000000000000000000000\n");
  // Serial.print("\n");
  
  // Serial.print("\n");
  // // Print uint64_t in binary by converting to String
  // for (size_t j =0; j< NO_OF_GIDS; j++)
  // {
  //    String binStr = "";
  //   for (int i = 63; i >= 0; i--) {
  //     binStr += ((displayData[j] >> i) & 1) ? '1' : '0';
  // }
  //     Serial.println(binStr);

  // }
 
  display.updateMatrix(displayData[0], 0);
  display.updateMatrix(displayData[1], 1);
  display.updateMatrix(displayData[2], 2);
  display.outputMatrix(0);
  display.outputMatrix(1);
  display.outputMatrix(2);
  
}

uint32_t convertStringToU64(String str)
{


  // First pad the bits with 0s to make it 128 bits long
  
  uint32_t val[4] = {strtoul(str.substring(0, 31).c_str(), nullptr, 2),strtoul(str.substring(31, 63).c_str(), nullptr, 2),strtoul(str.substring(63, 95).c_str(), nullptr, 2),strtoul(str.substring(95, 127).c_str(), nullptr, 2)};
  // for (unsigned int i = 0; i < str.length(); i++)
  // {
  //   val = val * 10;
  //   val = val + str[i] - '0';
  //   val = val ^ 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;
  // }
  frameToDisplays(val);
}
void setup()
{
  Serial.begin(9600);
  display.begin(DATA_PIN, STROBE_PIN, CLK_PIN, NO_OF_SEGS);
  ammeter.begin(A0, 4, A2, 5);
  ammeter.toHCS();

  display.setMatrix(matrix, NO_OF_GIDS);
  display.outputMatrix(0);
  convertStringToU64(frame);
  delay(100);
  
}

void loop()
{

  

  // while (Serial.available())
  // {
  display.outputMatrix(0);

  //   char c = Serial.read();
  //   Serial.print(c);
  //   if (c != '\n')
  //     readString += c;
  //   else if (firstRun) {
  //     firstRun = false;
  //     readString = "";
  //   }
  //   else if (c == '\n' && firstRun == false)
  //   {
  //     convertStringToU64(readString);
  //     readString = "";
  //     firstRun = true;
  //   }
  // }
}




// 0111110000000000011111000000000001111100000000000
// 0000001111100000000000111110000000000011111000000
// 0000000000011111000000000001111100000000000111110
// 11111100001111111111100000111111

// 0b00000000001111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
// 0b0000000000000000000000000011111000000000000000000000000000000000000000000000000000000000000000000000000000000000
// 0b0000000000000000000000000000000000000000001111100000000000000000000000000000000000000000000000000000000000000000
// 0b0000000000000000000000000000000000000000000000000000000000111110000000000000000000000000000000000000000000000000
// 0b0000000000000000000000000000000000000000000000000000000000000000000000000011111000000000000000000000000000000000
// 0b0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111100000000000000000
// 0b0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111110

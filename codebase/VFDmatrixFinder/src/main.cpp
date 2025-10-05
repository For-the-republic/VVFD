
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

int k = 0;
int j = 0;
uint8_t image[128] = {0};
String readString = "";
AmmeterVFD ammeter = AmmeterVFD();

shiftRegVFD display = shiftRegVFD();

// this is frame 231, roughly 7 seconds into the video "bad apple". it is used for testing to make sure the grid bit fiddling is done right
String frame = "111111111101111111111111100001111111111100000011111111111000001111111111110000111111001110000111111110000000011111111100";
// what this frame represents
//   seg1 |  seg2 |  seg3 || serial Data
//  ##### | ##### | .#### || ##########.####
//  ##### | ####. | ...## || #########....##
//  ##### | ###.. | ....# || ########......#
//  ##### | ####. | ....# || #########.....#
//  ##### | ##### | ....# || ##########....#
//  ####. | .###. | ...## || ####..###....##
//  ##### | ..... | ...## || #####........##

uint64_t segList[NO_OF_SEGS] =
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

void frameToDisplays(uint16_t *frame)
{
  size_t shiftAmount = 43;

  // Clear old data
  uint64_t displayData[NO_OF_GIDS] = {0};
  // this is specifically NOT a loop to make it as fast as possible
  displayData[0] = (((uint64_t)(frame[0] & 0xF800) << (shiftAmount)) |
                    ((uint64_t)(frame[1] & 0xF800) << (shiftAmount - 5)) |
                    ((uint64_t)(frame[2] & 0xF800) << (shiftAmount - 10)) |
                    ((uint64_t)(frame[3] & 0xF800) << (shiftAmount - 15)) |
                    ((uint64_t)(frame[4] & 0xF800) << (shiftAmount - 20)) |
                    ((uint64_t)(frame[5] & 0xF800) << (shiftAmount - 25)) |
                    ((uint64_t)(frame[6] & 0xE000) << (shiftAmount - 30)) |
                    ((uint64_t)(frame[6] & 0x1800) << (shiftAmount -30)));

  displayData[1] = (((uint64_t)(frame[0] & 0x07C0) << (shiftAmount + 5)) |
                    ((uint64_t)(frame[1] & 0x07C0) << (shiftAmount)) |
                    ((uint64_t)(frame[2] & 0x07C0) << (shiftAmount - 5)) |
                    ((uint64_t)(frame[3] & 0x07C0) << (shiftAmount - 10)) |
                    ((uint64_t)(frame[4] & 0x07C0) << (shiftAmount - 15)) |
                    ((uint64_t)(frame[5] & 0x07C0) << (shiftAmount - 20)) |
                    ((uint64_t)(frame[6] & 0x07C0) << (shiftAmount - 25)) |
                    ((uint64_t)(frame[6] & 0x00C0) << (shiftAmount - 25)));

  displayData[2] = (((uint64_t)(frame[0] & 0x003E) << (shiftAmount + 10)) |
                    ((uint64_t)(frame[1] & 0x003E) << (shiftAmount + 5)) |
                    ((uint64_t)(frame[2] & 0x003E) << (shiftAmount)) |
                    ((uint64_t)(frame[3] & 0x003E) << (shiftAmount - 5)) |
                    ((uint64_t)(frame[4] & 0x003E) << (shiftAmount - 10)) |
                    ((uint64_t)(frame[5] & 0x003E) << (shiftAmount - 15)) |
                    ((uint64_t)(frame[6] & 0x0038) << (shiftAmount - 20)) |
                    ((uint64_t)(frame[6] & 0x0006) << (shiftAmount - 20)));

  // bit masks over the grids to stop too much current being drawn
  // displayData[0] = (displayData[0]<<15 & 0b1111111111111111111111111111111111111111000011100000000000000000);
  // displayData[1] = (displayData[1]<<15 & 0b1111111111111111111111111111111111111111000011100000000000000000);
  // displayData[2] = (displayData[2]<<15 & 0b1111111111111111111111111111111111111111000011100000000000000000);
  // // now mask back over
  // add in the grids on the end
  displayData[0] = (displayData[0] | 0b0000000000000000000000000000000000000000000000000100000000000000);
  displayData[1] = (displayData[1] | 0b0000000000000000000000000000000000000000000000000010000000000000);
  displayData[2] = (displayData[2] | 0b0000000000000000000000000000000000000000000000000000001000000000);

  // now output the data and update the matrix. this is done after in case one bitmask takes slightly longer than the other
  
  // Serial.print(char(displayData[2]),BIN);

  display.updateMatrix(displayData[0], 0);
  display.updateMatrix(displayData[1], 1);
  display.updateMatrix(displayData[2], 2);
  display.outputMatrix(0);
  display.outputMatrix(1);
  display.outputMatrix(2);
}

uint16_t *convertStringToU64(String str)
{

  // with this method you dont have to worry about zeroes, and you just grab what you need
  static uint16_t val[7] = {
      (uint16_t)strtoul(str.substring(0, 16).c_str(), nullptr, 2),
      (uint16_t)strtoul(str.substring(16, 32).c_str(), nullptr, 2),
      (uint16_t)strtoul(str.substring(32, 48).c_str(), nullptr, 2),
      (uint16_t)strtoul(str.substring(48, 64).c_str(), nullptr, 2),
      (uint16_t)strtoul(str.substring(64, 96).c_str(), nullptr, 2),
      (uint16_t)strtoul(str.substring(96, 112).c_str(), nullptr, 2)};
  

for (size_t i = 0; i < 7; i++) {
  int start = i * 16;
  int end = start + 16;
  val[i] = (uint16_t)strtoul(str.substring(start, end).c_str(), nullptr, 2);
}
return val;

}
void setup()
{
  Serial.begin(115200);
  display.begin(DATA_PIN, STROBE_PIN, CLK_PIN, NO_OF_SEGS);
  ammeter.begin(A0, 4, A2, 5);
  ammeter.toHCS();

  display.setMatrix(matrix, NO_OF_GIDS);
  display.outputMatrix(0);
  frameToDisplays(convertStringToU64(frame));
}

void loop()
{

  while (Serial.available())
  {
  

    char c = Serial.read();
    Serial.print(c);
    if (c != '\n') {
      readString += c;
}
    else if (c == '\n')
    {
      frameToDisplays(convertStringToU64(readString));
      
      readString = "";

    }
  } 
    display.outputMatrix(0);
    display.outputMatrix(1);
    display.outputMatrix(2);

  
    

  }


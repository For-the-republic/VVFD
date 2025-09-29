#include <Arduino.h>
#include <shiftRegVFD.h>
#include <Wire.h>

const int DIN_PIN = 10;
const int CS_PIN = 9;
const int CLK_PIN = 8;
const int NO_OF_GIDS = 8;
const int NO_OF_SEGS = 8;
//this sets the locations of which grids you are updating
const int GRID1 = 0;
const int GRID2 = 1;
const int GRID3 = 2;

shiftRegVFD display = shiftRegVFD();

void displayImage(uint64_t image)
{
    int64_t display1[35] = {0};
    int64_t display2[35] = {0};
    int64_t display3[35] = {0};
    for size_t i = 0; i < NO_OF_GIDS*NO_OF_SEGS; i+=15 {
        display1[i:i+5] = image[i:i+5];
        display2[i+5:i] = image[i+5:i+10];
        display3[i+10:i] = image[i+10:i+15];
    }


        //converts the bad apple image into the VFD matrix format
    display.updateMatrix(display1, GRID1);
    display.updateMatrix(display2, GRID2);
    display.updateMatrix(display3, GRID3);
    display.outputList(display1);
    delay(3.32/3);
    display.outputList(display2);
    delay(3.32/3);
    display.outputList(display3);
    delay(3.32/3);
  
}
uint64_t convertStringToU64(String str)
{

  uint64_t val = 0;
  for (unsigned int i = 0; i < str.length(); i++)
  {
    val = val * 10;
    val = val + str[i] - '0';
  }
  return val;
}

void setup()
{
  Serial.begin(115200);
  display.begin(DIN_PIN, CS_PIN, CLK_PIN, 8, 8);
  display.setMatrix(*matrix, NO_OF_GIDS,NO_OF_SEGS);
}
int k = 0;
int j = 0;
int64_t image = 0;
String readString = "";
void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();
    Serial.print(c);
    if (c != '\n')
      readString += c;
    else
    {
      Serial.print(readString + "|");
      image = convertStringToU64(readString);
      displayImage(image);
      readString = "";
    }
  }
}

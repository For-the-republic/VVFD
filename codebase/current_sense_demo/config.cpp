#include <Arduino.h>
#include <SPI.h>
#include "config.h"

void setup_pins_peripherals(void) {
  for (size_t i = 0; i < CURRENT_SENSE_NO; i++) {
    pinMode(CURRENT_SENSE_PINS[i], INPUT);
  }

  pinMode(LCS_EN, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(LCS_EN, HIGH);

  delay(100);
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
}
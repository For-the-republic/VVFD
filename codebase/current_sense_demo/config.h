#ifndef CONFIG_H
#define CONFIG_H

#define SPI_CLOCK 100000
#define SHIFT_LATCH 10

#define HCS_EN 4
#define LCS_EN 5

#define CURRENT_SENSE_NO 4
#define HCS_3 A0
#define HCS_5 A1
#define LCS_3 A2
#define LCS_5 A3
const int CURRENT_SENSE_PINS[CURRENT_SENSE_NO] = {HCS_3, HCS_5, LCS_3, LCS_5};

#define DRIVER_PINS 64

// Current sense ADC min value for a segment
#define SEGMENT_CURRENT_TRESH 5

// Curent sense ADC min value for an associated grid and segment
#define GRID_SEGMENT_CURRENT_TRESH 11

// How many milliseconds to wait after setting a pin before reading the current sensor
#define SENSE_WAIT 10

void setup_pins_peripherals(void);

#endif 
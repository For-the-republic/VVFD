#include <Arduino.h>

#include "config.h"
#include "helpers.h"

// Reverse binary uint64_t pin printing to make it more user readable
#define REVERSE_BIN true 

void setup() {
  Serial.begin(115200);
  setup_pins_peripherals();

  uint64_t segments = get_segments();
  Serial.println("Segment pins:");
  serial_print_pin_id(REVERSE_BIN);
  serial_print_bin(&segments, sizeof(uint64_t), REVERSE_BIN);
  Serial.println();

  delay(1000); // Give some time for the user to read the segments

  print_combinations(segments);
}

void loop() {
  
}

// Read current for every pin to identify grids segments pins
// Segment pins will have a higher current sense ADC reading
// Returns a uint64_t, where on bits correspond to segment pins
uint64_t get_segments() {
  uint64_t segments = 0;

  for (uint8_t i = 0; i < DRIVER_PINS; i++) {
    uint64_t pin_mask = PIN(i);
    switch_pins(pin_mask);
    delay(SENSE_WAIT);

    if (analogRead(LCS_5) >= SEGMENT_CURRENT_TRESH) {
      segments |= pin_mask;
    }

    // Print the ADC reading of the current sense 
    // For testing / debugging
    // if (i % 8 == 0) {
    //   Serial.println();
    // }
    // Serial.print(analogRead(LCS_5));
    // Serial.print(", ");
  }

  return segments;
}

// Prints associated grids / segments and no connects
void print_combinations(uint64_t segments) {
  uint64_t grids = ~segments;
  uint64_t connected_segments = 0;
  uint64_t no_connect_grids = 0;

  for (uint8_t i = 0; i < DRIVER_PINS; i++) {
    if (bitRead(grids, i)) {
      uint64_t grid_pin_mask = PIN(i);
      uint64_t associated_segments = 0;

      // Find assoicated segments for this grid
      for (uint8_t j = 0; j < DRIVER_PINS; j++) {
        if (bitRead(segments, j)) {
          uint64_t segment_pin_mask = PIN(j);
          switch_pins(grid_pin_mask | segment_pin_mask);
          delay(SENSE_WAIT);

          if (analogRead(LCS_5) >= GRID_SEGMENT_CURRENT_TRESH) {
            associated_segments |= segment_pin_mask;
            connected_segments |= segment_pin_mask;
          }
        }
      }

      // Print associated segments for this grid
      if (associated_segments == 0) {
        no_connect_grids |= grid_pin_mask;
      } else {
        serial_print_associated(i, "(Grid)", "Has connected segments:", associated_segments, REVERSE_BIN);
      }
    }
  }

  // Print no connect pins
  uint64_t nc = ~connected_segments & segments | no_connect_grids & grids;
  Serial.println("No connect pins:");
  serial_print_pin_id(REVERSE_BIN);
  serial_print_bin(&nc, sizeof(uint64_t), REVERSE_BIN);
  Serial.println();
}
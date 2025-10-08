#include <Arduino.h>

#include "config.h"
#include "helpers.h"

// Reverse binary uint64_t pin printing to make it more user readable
#define REVERSE_BIN true 
#define PAD_BYTES true 

void setup() {
  Serial.begin(115200);

  setup_pins_peripherals();
  switch_pins(PIN(1) | PIN(41));

  // uint64_t grids = get_grids();
  // Serial.println("Grid pins:");
  // serial_print_pin_id(REVERSE_BIN, PAD_BYTES);
  // serial_print_bin(&grids, sizeof(uint64_t), REVERSE_BIN, PAD_BYTES);
  // Serial.println();

  // delay(1000); // Give some time for the user to read the segments

  // print_combinations(segments);

  // Grid order
  //  41 42 43 44 40

  // switch_pins(PIN(1) | PIN(41));
  switch_pins(0);
  delay(SENSE_WAIT);
  Serial.println(analogRead(CURRENT_SENSE_PIN));

  // switch_pins(PIN(1) | PIN(40));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(5));

  // switch_pins(PIN(1) | PIN(41));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(5));

  // switch_pins(PIN(1) | PIN(42));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(5));


  // switch_pins(PIN(1) | PIN(40));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(5));


  // switch_pins(PIN(1) | PIN(64));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(5));

  // switch_pins(PIN(1));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(5));

  // switch_pins(PIN(64));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(5));

  // switch_pins(PIN(1) | PIN(40));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(CURRENT_SENSE_PIN));

  // switch_pins(PIN(40));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(CURRENT_SENSE_PIN));

  // switch_pins(PIN(63));
  // delay(SENSE_WAIT);
  // Serial.println(analogRead(CURRENT_SENSE_PIN));


}

void loop() {
  
}

// Read current for every pin to identify grids segments pins
// Segment pins will have a higher current sense ADC reading
// Returns a uint64_t, where on bits correspond to segment pins
uint64_t get_grids() {
  uint64_t grids = 0;

  for (uint8_t i = 0; i < DRIVER_PINS; i++) {
    uint64_t pin_mask = PIN(i) | PIN(KNOWN_SEGMENT);
    switch_pins(pin_mask);
    delay(SENSE_WAIT);

    int current = analogRead(CURRENT_SENSE_PIN);
    if (current >= 600 && current <= 800) {
      grids |= pin_mask;
    }

    // Print the ADC reading of the current sense 
    // For testing / debugging
    if (i % 8 == 0) {
      Serial.println();
    }
    Serial.print(analogRead(CURRENT_SENSE_PIN));
    Serial.print(", ");
  }

  return grids;
}

// Prints associated grids / segments and no connects
void print_combinations(uint64_t grids) {
  uint64_t segments = ~grids;
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

          if (analogRead(CURRENT_SENSE_PIN) >= GRID_SEGMENT_CURRENT_TRESH) {
            associated_segments |= segment_pin_mask;
            connected_segments |= segment_pin_mask;
          }
        }
      }

      // Print associated segments for this grid
      if (associated_segments == 0) {
        no_connect_grids |= grid_pin_mask;
      } else {
        serial_print_associated(i, "(Grid)", "Has connected segments:", associated_segments, REVERSE_BIN, PAD_BYTES);
      }
    }
  }

  // Print no connect pins
  uint64_t nc = ~connected_segments & segments | no_connect_grids & grids;
  Serial.println("No connect pins:");
  serial_print_pin_id(REVERSE_BIN, PAD_BYTES);
  serial_print_bin(&nc, sizeof(uint64_t), REVERSE_BIN, PAD_BYTES);
  Serial.println();
}
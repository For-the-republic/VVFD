#include <Arduino.h>
#include <SPI.h>

#include "helpers.h"
#include "config.h"

// Reverses an index unless disable is true
size_t _rev_index(size_t index, size_t len, bool disable) {
  if (disable) {
    return index;
  } else {
    return len - index - 1;
  }
}

// Populates a bytes array with bytes from num in either little or big endian order
// Machine endianness wont break this
// void get_bytes(uint64_t num, uint8_t* bytes_array, bool little_endian) {
//   for (size_t i = 0; i < sizeof(uint64_t); i++) {
//     size_t array_index = _rev_index(i, sizeof(uint64_t), little_endian);
//     bytes_array[array_index] = (uint8_t)(num >> i * 8);
//   }
// }

// Populates a bytes array with bytes from a num in either little or big endian order
// len is the number of bytes in bytes
// Assumes the hardware uses lttle endian
void get_bytes(void* bytes, size_t len, uint8_t* bytes_array, bool little_endian) {
  for (size_t i = 0; i < len; i++) {
    uint8_t this_byte = ((uint8_t*)bytes)[_rev_index(i, len, little_endian)];
    bytes_array[i] = this_byte;
  }
}

// Transfers a 64 bit pin mask over SPI into the shift registers
void switch_pins(uint64_t pin_mask) {
  uint8_t bytes_array[8] = {0};
  get_bytes(&pin_mask, sizeof(uint64_t), bytes_array, false);

  SPI.transfer(bytes_array, sizeof(uint64_t)); // Trransfer

  // Latch
  delayMicroseconds(10);
  digitalWrite(SHIFT_LATCH, HIGH);
  delayMicroseconds(10);
  digitalWrite(SHIFT_LATCH, LOW);
  delayMicroseconds(10);
}

// Prints bytes in binary
// The least significant bit will be printed on the left if print_backwards = true
// Assumes the hardware uses lttle endian
void serial_print_bin(void* bytes, size_t len, bool print_backwards, bool pad_bytes) {
  for (uint8_t i = 0; i < len; i++) {
    uint8_t this_byte = ((uint8_t*)bytes)[_rev_index(i, len, print_backwards)];

    for (uint8_t j = 0; j < 8; j++) {
      Serial.print(bitRead(this_byte, _rev_index(j, 8, print_backwards)));

    }

    if (pad_bytes) {
      Serial.print(" ");
    }
  }

  Serial.println();
}

// Prints identifier to make pin binary masks more user readable
void serial_print_pin_id(bool reversed, bool pad_bytes) {
  char* start_msg;
  char* end_msg;
  if (!reversed) {
    start_msg = "<";
    end_msg = " PIN 0";
  } else {
    start_msg = "PIN 0 ";
    end_msg = ">";
  }

  uint8_t len = DRIVER_PINS - 7;
  if (pad_bytes) {
    len += 7;
  }

  Serial.print(start_msg);
  for (uint8_t i = 0; i < len; i++) {
    Serial.print("-");
  }
  Serial.println(end_msg);
}

// Helper for printing associated grids / segments
void serial_print_associated(uint8_t pin_no, char* id_s, char* msg, uint64_t pin_mask, bool reverse_bin, bool pad_bytes) {
  char buffer[50];
  sprintf(buffer, "Pin: %d %s %s", pin_no, id_s, msg);
  Serial.println(buffer);
  serial_print_bin(&pin_mask, sizeof(uint64_t), reverse_bin, pad_bytes);
  Serial.println();
}
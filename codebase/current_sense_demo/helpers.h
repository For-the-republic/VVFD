#ifndef HELPERS_H
#define HELPERS_H

#define PIN(pos) ((uint64_t)1 << (pos))

void get_bytes(void* bytes, size_t len, uint8_t* bytes_array, bool little_endian);
void switch_pins(uint64_t num);
void serial_print_bin(void* bytes, size_t len, bool print_backwards);
void serial_print_pin_id(bool reversed);
void serial_print_associated(uint8_t pin_no, char* id_s, char* msg, uint64_t pin_mask, bool reverse_bin);

#endif
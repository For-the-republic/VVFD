#ifndef acGeneratorVFD_h
#define acGeneratorVFD_h
#include "Arduino.h"

// this is a way to generate the ac waveofrm for the wires without it being in the main loop
// it should only be called once at the start

class acGeneratorVFD {

    public:

    acGeneratorVFD();
    void begin(uint8_t acPin1, uint8_t acPin2, bool state);

    private:
    uint8_t _acPin1;
    uint8_t _acPin2;
    bool _state;

};
#endif
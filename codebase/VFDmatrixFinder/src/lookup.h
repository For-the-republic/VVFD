#ifndef LOOKUP_H
#define LOOKUP_H

#include "Arduino.h"
#include "acGeneratorVFD.h"
#include "wire.h"
class lookupTable {
    public:
    lookupTable();
    uint8_t Lookup(char letter);
    private:
};
#endif


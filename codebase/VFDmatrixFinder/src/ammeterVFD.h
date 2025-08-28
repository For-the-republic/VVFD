#ifndef AmmeterVFD_h
#define AmmeterVFD_h
#include "Arduino.h"
// this library is how you get the ammeter values from the pins without being in the main loop.
// it will make the main cpp file a whole lot cleanter

class AmmeterVFD {
public:
    AmmeterVFD();
    void begin(int hcsIN, int hcsEN, int lcsIN, int lcsEN);
    void toLCS();
    void toHCS();
    float readCurrent();



    private:
    uint8_t _hcsIN;
    uint8_t _hcsEN;
    uint8_t _lcsIN;
    uint8_t _lcsEN;
    float _current;
    float _prevCurrent;
    bool _mode;
    long _lastReadTime;
    const long _delayTime = 500; //delay time before the next read
};
#endif
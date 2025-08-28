#include "Arduino.h"
#include "ammeterVFD.h"
#include "wire.h"


AmmeterVFD::AmmeterVFD() {
    _current = 0;
    _prevCurrent = 0;
    _mode = 0; //start in low current mode
    // 0 is low current mode, 1 is high current
    _lastReadTime = 0;
}
void AmmeterVFD::begin(int hcsIN, int hcsEN, int lcsIN, int lcsEN) {
    _hcsIN = hcsIN;
    _hcsEN = hcsEN;
    _lcsIN = lcsIN;
    _lcsEN = lcsEN;

    pinMode(_hcsEN, OUTPUT);
    pinMode(_lcsEN, OUTPUT);
    pinMode(_hcsIN, INPUT);
    pinMode(_lcsIN, INPUT);

    digitalWrite(_hcsEN, HIGH); //disable high current sensor
    digitalWrite(_lcsEN, LOW); //enable low current sensor
}
    void AmmeterVFD::toLCS()
    {
        digitalWrite(_hcsEN, HIGH); //disable high current sensor
        digitalWrite(_lcsEN, LOW); //enable low current sensor
        _mode = 0;
    }
    void AmmeterVFD:: toHCS()
    {
        digitalWrite(_hcsEN, LOW); //enable high current sensor
        digitalWrite(_lcsEN, HIGH); //disable low current sensor
        _mode = 1;
    }
    float AmmeterVFD:: readCurrent()
    {
        if(_mode == 0) {
            return analogRead(_lcsIN);
        } else {
            return analogRead(_hcsIN);
        }
    }

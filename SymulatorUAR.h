#pragma once
#include "ARX.h"
#include "Reg_PID.h"

class SymulatorUAR {
public:
    SymulatorUAR(RegulatorPID& regulator, ModelARX& arx);

    double symuluj(double w_i);
    void reset();
    double getOpoznionaWartoscZmierzona() const { return _y_opozniona; }
    double getUchyb() const { return _uchyb; }
    double getSterowanie() const { return _arx.getUzasymulowane(); }

private:
    RegulatorPID& _regulator;
    ModelARX& _arx;

    double _y_opozniona;

    double _uchyb;
    double _u;
};

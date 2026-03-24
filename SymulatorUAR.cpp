#include "SymulatorUAR.h"

SymulatorUAR::SymulatorUAR(RegulatorPID& regulator, ModelARX& arx)
    : _regulator(regulator), _arx(arx), _y_opozniona(0.0), _uchyb(0.0), _u(0.0)
{
}

double SymulatorUAR::symuluj(double w_i)
{
    _uchyb = w_i - _y_opozniona;

    _u = _regulator.symuluj(_uchyb);

    double y_i = _arx.symuluj(_u);
    _y_opozniona = y_i;

    return y_i;
}

void SymulatorUAR::reset()
{
    _y_opozniona = 0.0;
    _uchyb = 0.0;
    _u = 0.0;

    _regulator.resetPamieci();
    _arx.reset();
}

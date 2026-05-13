#include "Reg_PID.h"
#include <cmath>

RegulatorPID::RegulatorPID(double Kp, double Ti, double Td, double Tp, double U_min, double U_max) {
    ustawParametry(Kp, Ti, Td, U_min, U_max, Tp);
    _liczCalk = LiczCalk::Zew;
    resetPamieci();
}

void RegulatorPID::ustawParametry(double Kp, double Ti, double Td, double U_min, double U_max, double Tp) {
    _Kp = Kp;
    _Ti = Ti;
    _Td = Td;
    _Tp = Tp;
    _U_min = U_min;
    _U_max = U_max;
}

void RegulatorPID::resetPamieci() {
    _Suma_uchyb_wew = 0.0;
    _Suma_uchyb_zew = 0.0;
    _Uchyb_poprz = 0.0;
    _ostatnieSkladowe = {0.0, 0.0, 0.0};
}

void RegulatorPID::resetCalki() {
    _Suma_uchyb_wew = 0.0;
    _Suma_uchyb_zew = 0.0;

}

void RegulatorPID::setLiczCalk(LiczCalk tryb) {
    if (_liczCalk != tryb) {
        constexpr double EPS = std::numeric_limits<double>::epsilon();


        if (tryb == LiczCalk::Wew) {

            if (_Ti > EPS) _Suma_uchyb_wew = _Suma_uchyb_zew / _Ti;
            else _Suma_uchyb_wew = 0.0;
        } else {

            if (_Ti > EPS) _Suma_uchyb_zew = _Suma_uchyb_wew * _Ti;
            else _Suma_uchyb_zew = 0.0;
        }
        _liczCalk = tryb;
    }
}

void RegulatorPID::setStalaCalk(double Ti) {
    _Ti = Ti;
}

double RegulatorPID::symuluj(double uchyb) {
    constexpr double EPS = std::numeric_limits<double>::epsilon();

    // 1. Człon Proporcjonalny (P)
    double P = _Kp * uchyb;

    // 2. Człon Różniczkujący (D) - bez wymnażania przez Kp
    double D = 0.0;
    if (_Tp > EPS) {
        D = _Td * (uchyb - _Uchyb_poprz) / _Tp;
    }

    // 3. Człon Całkujący (I) - bez wymnażania przez Kp
    double I_tent = 0.0;
    double S_tent = 0.0;

    if (_Ti > EPS) {
        if (_liczCalk == LiczCalk::Wew) {
            double przyrost = (uchyb * _Tp) / _Ti;
            S_tent = _Suma_uchyb_wew + przyrost;
            I_tent = S_tent;
        } else {
            S_tent = _Suma_uchyb_zew + (uchyb * _Tp);
            I_tent = S_tent / _Ti;
        }
    } else {
        I_tent = 0.0;
        S_tent = (_liczCalk == LiczCalk::Wew) ? _Suma_uchyb_wew : _Suma_uchyb_zew;
    }

    _ostatnieSkladowe = { P, I_tent, D };

    // Saturacja
    double u_niesatur = P + I_tent + D;
    double u_satur = u_niesatur;

    if (u_satur > _U_max) u_satur = _U_max;
    else if (u_satur < _U_min) u_satur = _U_min;

    // Anti-windup
    bool nasycenie = (std::abs(u_satur - u_niesatur) > EPS);
    if (!nasycenie) {
        if (_liczCalk == LiczCalk::Wew) _Suma_uchyb_wew = S_tent;
        else _Suma_uchyb_zew = S_tent;
    }

    _Uchyb_poprz = uchyb;
    return u_satur;
}

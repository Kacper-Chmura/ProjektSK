#pragma once
#include <vector>
#include <limits>

class RegulatorPID {
public:
    enum class LiczCalk { Wew, Zew };
    struct Skladowe { double P, I, D; };

    RegulatorPID(double Kp, double Ti = 0.0, double Td = 0.0, double Tp = 1.0, double U_min = -10, double U_max = 10);

    void ustawParametry(double Kp, double Ti, double Td, double U_min, double U_max, double Tp);


    double symuluj(double uchyb);

    void resetPamieci();
    void resetCalki();
    void setUchybPoprzedni(double e) { _Uchyb_poprz = e; }

    void setLiczCalk(LiczCalk tryb);
    void setStalaCalk(double Ti);

    Skladowe getOstatnieSkladowe() const { return _ostatnieSkladowe; }

    double getKp() const { return _Kp; }
    double getTi() const { return _Ti; }
    double getTd() const { return _Td; }
    double getTp() const { return _Tp; }
    double getUMin() const { return _U_min; }
    double getUMax() const { return _U_max; }
    LiczCalk getLiczCalk() const { return _liczCalk; }

private:
    double _Kp, _Ti, _Td, _Tp;
    double _U_min, _U_max;

    double _Suma_uchyb_wew;
    double _Suma_uchyb_zew;
    double _Uchyb_poprz;

    LiczCalk _liczCalk;
    Skladowe _ostatnieSkladowe;
};

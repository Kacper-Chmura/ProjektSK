#include "MenadzerSymulacji.h"
#include "ZarzadzanieCzasem.h"
#include <vector>

MenadzerSymulacji::MenadzerSymulacji(double Kp, double Ti, double Td, double Tp, double U_min, double U_max,
                                     const std::vector<double>& A, const std::vector<double>& B, int k, double pozsz, QObject *parent)
    : QObject(parent),
    _regulator(Kp, Ti, Td, Tp, U_min, U_max),
    _model(A, B, k, pozsz),
    _symulator(_regulator, _model),
    _gen_sinusoidalny(1.0, 10.0, 0.0),
    _gen_prostokatny(1.0, 10.0, 0.5, 0.0),
    _aktywny_generator(TypGeneratora::Sinusoidalny),
    _ostatnia_wartosc_zadana(0.0),
    _ostatnia_wartosc_regulowana(0.0),
    _symulacja_uruchomiona(false)
{
    _czasownik = new ZarzadzanieCzasem(this, this);
}

void MenadzerSymulacji::wykonajKrokSymulacji(double czas)
{
    if (!_symulacja_uruchomiona) return;

    double wartoscZadana = 0.0;
    if (_aktywny_generator == TypGeneratora::Sinusoidalny) {
        wartoscZadana = _gen_sinusoidalny.generuj(czas);
    } else {
        wartoscZadana = _gen_prostokatny. generuj(czas);
    }
    _ostatnia_wartosc_zadana = wartoscZadana;

    double y = _symulator.symuluj(wartoscZadana);
    _ostatnia_wartosc_regulowana = y;

    double u = _symulator.getSterowanie();
    double e = _symulator.getUchyb();
    auto skladowe = _regulator.getOstatnieSkladowe();

    emit noweDataReady(czas, wartoscZadana, y, u, e, skladowe);
}

RegulatorPID::Skladowe MenadzerSymulacji::getSkladowePID() const {
    return _regulator.getOstatnieSkladowe();
}
void MenadzerSymulacji::resetCalkiPID() {
    _regulator.resetCalki();
}
void MenadzerSymulacji::resetPamieciPID() {
    _regulator.resetPamieci();
}
void MenadzerSymulacji::setNastawyPID(double Kp, double Ti, double Td, double U_min, double U_max, double Tp) {
    _regulator.ustawParametry(Kp, Ti, Td, U_min, U_max, Tp);
}
void MenadzerSymulacji::setLiczCalkPID(RegulatorPID::LiczCalk tryb) {
    _regulator.setLiczCalk(tryb);
}
void MenadzerSymulacji::setStalaCalkPID(double Ti) {
    _regulator.setStalaCalk(Ti);
}

void MenadzerSymulacji::resetSymulacji() {
    _symulator.reset();
    _symulacja_uruchomiona = false;
    _czasownik->reset();
}
void MenadzerSymulacji::startSymulacji() {
    _symulacja_uruchomiona = true;
    _czasownik->start();
}
void MenadzerSymulacji::stopSymulacji() {
    _symulacja_uruchomiona = false;
    _czasownik->stop();
    // Niepotrzebnie: _regulator.resetCalki();
}
bool MenadzerSymulacji::czySymulacjaUruchomiona() const { return _symulacja_uruchomiona; }

void MenadzerSymulacji::setTypGeneratora(TypGeneratora typ) { _aktywny_generator = typ; }
void MenadzerSymulacji::setParametryGeneratoraSinusoidalnego(double amplituda, double okres, double skladowaStala) {
    _gen_sinusoidalny.ustawParametry(amplituda, okres, skladowaStala);
}
void MenadzerSymulacji::setParametryGeneratoraProstokatnego(double amplituda, double okres, double wypelnienie, double skladowaStala) {
    _gen_prostokatny.ustawParametry(amplituda, okres, skladowaStala);
    _gen_prostokatny.setWypelnienie(wypelnienie);
}
double MenadzerSymulacji::getWartoscZadana() const { return _ostatnia_wartosc_zadana; }

void MenadzerSymulacji::setParametryARX(const std::vector<double>& A, const std::vector<double>& B, int k, double pozsz) {
    std::vector<double> A_copy = A;
    std::vector<double> B_copy = B;
    _model.setA(A_copy);
    _model.setB(B_copy);
    _model.setk(k);
    _model.setpozsz(pozsz);
}
void MenadzerSymulacji::resetModelARX() { _model.reset(); }
void MenadzerSymulacji::setOgraniczeniaARX(bool wlaczone) { _model.setOgraniczenia(wlaczone); }
void MenadzerSymulacji::setOgraniczeniaSterowania(double min, double max) { _model.setOgraniczeniaSterowania(min, max); }
void MenadzerSymulacji::setOgraniczeniaRegulowanej(double min, double max) { _model.setOgraniczeniaRegulowanej(min, max); }

double MenadzerSymulacji::getSterowanie() const { return _symulator.getSterowanie(); }
double MenadzerSymulacji::getUchyb() const { return _symulator.getUchyb(); }

double MenadzerSymulacji::getGenAmplituda() const {
    if (_aktywny_generator == TypGeneratora::Sinusoidalny)
        return _gen_sinusoidalny.getAmplituda();
    return _gen_prostokatny.getAmplituda();
}
double MenadzerSymulacji::getGenOkres() const {
    if (_aktywny_generator == TypGeneratora::Sinusoidalny)
        return _gen_sinusoidalny.getOkres();
    return _gen_prostokatny.getOkres();
}
double MenadzerSymulacji::getGenWypelnienie() const {
    return _gen_prostokatny.getWypelnienie();
}
double MenadzerSymulacji::getGenSkladowaStala() const {
    if (_aktywny_generator == TypGeneratora::Sinusoidalny)
        return _gen_sinusoidalny.getSkladowaStala();
    return _gen_prostokatny.getSkladowaStala();
}

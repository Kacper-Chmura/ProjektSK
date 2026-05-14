#pragma once
#include "Reg_PID.h"
#include "ARX.h"
#include "GeneratorSygnalu.h"
#include "SymulatorUAR.h"
#include <vector>
#include <QObject>

class ZarzadzanieCzasem;

class MenadzerSymulacji : public QObject
{
    Q_OBJECT

public:
    enum class TypGeneratora { Sinusoidalny, Prostokatny };

    MenadzerSymulacji(double Kp, double Ti, double Td, double Tp, double U_min, double U_max,
                      const std::vector<double>& A, const std::vector<double>& B, int k, double pozsz, QObject *parent = nullptr);

    void wykonajKrokSymulacji(double czas);

    void startSymulacji();
    void stopSymulacji();
    void resetSymulacji();
    bool czySymulacjaUruchomiona() const;
    void ustawAktywny(bool aktywny) { _symulacja_uruchomiona = aktywny; }

    double getWartoscZadana() const;
    double getWartoscRegulowana() const { return _ostatnia_wartosc_regulowana; }
    double getSterowanie() const;
    double getUchyb() const;

    RegulatorPID::Skladowe getSkladowePID() const;
    void resetCalkiPID();
    void resetPamieciPID();

    void setTypGeneratora(TypGeneratora typ);
    void setParametryGeneratoraSinusoidalnego(double amplituda, double okres, double skladowaStala);
    void setParametryGeneratoraProstokatnego(double amplituda, double okres, double wypelnienie, double skladowaStala);

    void setNastawyPID(double Kp, double Ti, double Td, double U_min, double U_max, double Tp);
    void setLiczCalkPID(RegulatorPID::LiczCalk tryb);
    void setStalaCalkPID(double Ti);

    void setParametryARX(const std::vector<double>& A, const std::vector<double>& B, int k, double pozsz);
    void resetModelARX();

    void setOgraniczeniaARX(bool wlaczone);
    void setOgraniczeniaSterowania(double min, double max);
    void setOgraniczeniaRegulowanej(double min, double max);

    enum class TrybPracy { Stacjonarny, SiecRegulator, SiecObiekt };
    void setTrybPracy(TrybPracy tryb);

    void aktualizujZSieciObiekt(double y);
    void aktualizujZSieciRegulator(double czas, double w, double u);
    ZarzadzanieCzasem* getZarzadzanieCzasem() { return _czasownik; }

    bool getOgraniczeniaARX() const { return _model. ograniczenia(); }
    double getUMinARX() const { return _model. getUMin(); }
    double getUMaxARX() const { return _model.getUMax(); }
    double getYMinARX() const { return _model.getYMin(); }
    double getYMaxARX() const { return _model.getYMax(); }

    RegulatorPID* getPID()   { return &_regulator; }
    ModelARX*     getARX()   { return &_model; }

    TypGeneratora getTypGeneratora() const { return _aktywny_generator; }
    double getGenAmplituda() const;
    double getGenOkres() const;
    double getGenWypelnienie() const;
    double getGenSkladowaStala() const;

    double getOstatniCzasSieci() const { return _ostatni_czas_sieci; }

signals:
    void noweDataReady(double czas, double y_zad, double y, double u, double e,
                       RegulatorPID::Skladowe skladowe);
    void wyslijRamkeRegulatora(double czas, double w, double u);
    void wyslijRamkeObiektu(double czas, double y);
    void sygnalWydajnosci(bool wyrabiaSie);
private:
    RegulatorPID _regulator;
    ModelARX _model;
    SymulatorUAR _symulator;

    SygnalSinusoidalny _gen_sinusoidalny;
    SygnalProstokatny _gen_prostokatny;
    TypGeneratora _aktywny_generator;

    double _ostatnia_wartosc_zadana;
    double _ostatnia_wartosc_regulowana;
    bool _symulacja_uruchomiona;
    double _ostatni_czas_sieci = 0.0;

    ZarzadzanieCzasem* _czasownik;
    TrybPracy _trybPracy = TrybPracy::Stacjonarny;
    bool _czy_dane_dotarly = true;
};

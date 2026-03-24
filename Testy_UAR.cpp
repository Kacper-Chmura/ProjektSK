// TU includy:

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include "ARX.h"
#include "Reg_PID.h"
#include "SymulatorUAR.h"
#include "TestUtils.h"

// GLOBALNE LICZNIKI
extern int total_tests;
extern int passed_tests;

#define DEBUG

#ifdef DEBUG

//Funkcje pomocnicze dla testów:

void raportBleduSekwencji_local(std::vector<double>& spodz, std::vector<double>& fakt)
{
    constexpr size_t PREC = 3;
    std::cerr << std::fixed << std::setprecision(PREC);
    std::cerr << "  Spodziewany:\t";
    for (auto& el : spodz)
        std::cerr << el << ", ";
    std::cerr << "\n  Faktyczny:\t";
    for (auto& el : fakt)
        std::cerr << el << ", ";
    std::cerr << std::endl << std::endl;
}

bool porownanieSekwencji_local(std::vector<double>& spodz, std::vector<double>& fakt)
{
    constexpr double TOL = 1e-3;
    bool result = fakt.size() == spodz.size();
    for (size_t i = 0; result && i < fakt.size(); i++)
        result = fabs(fakt[i] - spodz[i]) < TOL;
    return result;
}

void myAssert(std::vector<double>& spodz, std::vector<double>& fakt, std::string nazwa = "Test")
{
    total_tests++;
    if (porownanieSekwencji_local(spodz, fakt)) {
        passed_tests++;
    }
    else
    {
        std::cerr << "[FAIL] " << nazwa << "\n";
        raportBleduSekwencji_local(spodz, fakt);
    }
}

// TESTY ARX

namespace TESTY_ModelARX
{
void wykonaj_testy();
void test_brakPobudzenia();
void test_skokJednostkowy_1();
void test_skokJednostkowy_2();
void test_skokJednostkowy_3();
}

void TESTY_ModelARX::wykonaj_testy()
{
    test_brakPobudzenia();
    test_skokJednostkowy_1();
    test_skokJednostkowy_2();
    test_skokJednostkowy_3();
}

void TESTY_ModelARX::test_brakPobudzenia()
{
    try
    {

        ModelARX instancjaTestowa({ -0.4 }, { 0.6 }, 1, 0.0);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "ModelARX: Brak pobudzenia");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_ModelARX::test_skokJednostkowy_1()
{
    try
    {
        ModelARX instancjaTestowa({ -0.4 }, { 0.6 }, 1, 0.0);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0, 0, 0.6, 0.84, 0.936, 0.9744, 0.98976, 0.995904, 0.998362,
                      0.999345, 0.999738, 0.999895, 0.999958, 0.999983, 0.999993, 0.999997, 0.999999,
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "ModelARX: Skok 1");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_ModelARX::test_skokJednostkowy_2()
{
    try
    {
        ModelARX instancjaTestowa({ -0.4 }, { 0.6 }, 2, 0.0);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0, 0, 0, 0.6, 0.84, 0.936, 0.9744, 0.98976, 0.995904, 0.998362, 0.999345, 0.999738, 0.999895, 0.999958, 0.999983, 0.999993, 0.999997, 0.999999, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "ModelARX: Skok 2");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_ModelARX::test_skokJednostkowy_3()
{
    try
    {
        ModelARX instancjaTestowa({ -0.4,0.2 }, { 0.6, 0.3 }, 2, 0.0);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0, 0, 0, 0.6, 1.14, 1.236, 1.1664, 1.11936, 1.11446, 1.12191, 1.12587, 1.12597, 1.12521, 1.12489, 1.12491, 1.12499, 1.12501, 1.12501, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125 };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "ModelARX: Skok 3");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}


// TESTY PID

namespace TESTY_RegulatorPID
{
void wykonaj_testy();
void test_P_brakPobudzenia();
void test_P_skokJednostkowy();
void test_PI_skokJednostkowy_1();
void test_PI_skokJednostkowy_2();
void test_PID_skokJednostkowy();
void test_PI_skokJednostkowy_3();
}

void TESTY_RegulatorPID::wykonaj_testy()
{
    test_P_brakPobudzenia();
    test_P_skokJednostkowy();
    test_PI_skokJednostkowy_1();
    test_PI_skokJednostkowy_2();
    test_PID_skokJednostkowy();
    test_PI_skokJednostkowy_3();
}

void TESTY_RegulatorPID::test_P_brakPobudzenia()
{
    try
    {
        RegulatorPID instancjaTestowa(0.5);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "PID: Brak pobudzenia");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_RegulatorPID::test_P_skokJednostkowy()
{
    try
    {
        RegulatorPID instancjaTestowa(0.5);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
                      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "PID: Skok P");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_RegulatorPID::test_PI_skokJednostkowy_1()
{
    try
    {
        RegulatorPID instancjaTestowa(0.5, 1.0, 0.0, 1.0, -100.0, 100.0);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5, 11.5, 12.5,
                      13.5, 14.5, 15.5, 16.5, 17.5, 18.5, 19.5, 20.5, 21.5, 22.5, 23.5,
                      24.5, 25.5, 26.5, 27.5, 28.5, 29.5 };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "PID: Skok PI 1");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_RegulatorPID::test_PI_skokJednostkowy_2()
{
    try
    {
        RegulatorPID instancjaTestowa(0.5, 10.0);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0, 0.6, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9,
                      2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4 };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "PID: Skok PI 2");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_RegulatorPID::test_PID_skokJednostkowy()
{
    try
    {
        RegulatorPID instancjaTestowa(0.5, 10.0, 0.2);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0, 0.8, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2, 2.1,
                      2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4 };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "PID: Skok PID");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_RegulatorPID::test_PI_skokJednostkowy_3()
{
    try
    {
        RegulatorPID instancjaTestowa(0.5, 10.0);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0, 0.6, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.8, 2, 2.2, 2.4,
                      2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 2.35, 2.45, 2.55, 2.65, 2.75, 2.85 };

        for (int i = 0; i < LICZ_ITER; i++)
        {
            if (i == LICZ_ITER * 1 / 5)
                instancjaTestowa.setLiczCalk(RegulatorPID::LiczCalk::Wew);
            if (i == LICZ_ITER * 2 / 5)
                instancjaTestowa.setStalaCalk(5.0);
            if (i == LICZ_ITER * 3 / 5)
                instancjaTestowa.setLiczCalk(RegulatorPID::LiczCalk::Zew);
            if (i == LICZ_ITER * 4 / 5)
                instancjaTestowa.setStalaCalk(10.0);
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);
        }

        myAssert(spodzSygWy, faktSygWy, "PID: Zmiana parametrow w locie");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}


namespace TESTY_ProstyUAR
{
void wykonaj_testy();
void test_UAR_1_brakPobudzenia();
void test_UAR_1_skokJednostkowyPID();
void test_UAR_2_skokJednostkowyPID();
void test_UAR_3_skokJednostkowyPID();
}

void TESTY_ProstyUAR::wykonaj_testy()
{
    test_UAR_1_brakPobudzenia();
    test_UAR_1_skokJednostkowyPID();
    test_UAR_2_skokJednostkowyPID();
    test_UAR_3_skokJednostkowyPID();
}

void TESTY_ProstyUAR::test_UAR_1_brakPobudzenia()
{
    try
    {

        RegulatorPID testPID(0.5, 5.0, 0.2);
        ModelARX testARX({ -0.4 }, { 0.6 }, 1, 0.0);
        SymulatorUAR instancjaTestowa(testPID, testARX);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "UAR: Brak pobudzenia");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_ProstyUAR::test_UAR_1_skokJednostkowyPID()
{
    try
    {
        RegulatorPID testPID(0.5, 5.0, 0.2);
        ModelARX testARX({ -0.4 }, { 0.6 }, 1, 0.0);
        SymulatorUAR instancjaTestowa(testPID, testARX);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0.0, 0.0, 0.54, 0.756, 0.6708, 0.64008, 0.729, 0.810437, 0.834499,
            0.843338, 0.8664, 0.8936, 0.911886, 0.923312, 0.93404, 0.944929,
            0.954065, 0.961042, 0.966815, 0.971965, 0.97642, 0.980096, 0.983143,
            0.985741, 0.987964, 0.989839, 0.991411, 0.992739, 0.993865, 0.994818
        };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "UAR: Skok 1");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_ProstyUAR::test_UAR_2_skokJednostkowyPID()
{
    try
    {
        RegulatorPID testPID(0.5, 5.0, 0.2);
        ModelARX testARX({ -0.4 }, { 0.6 }, 2, 0.0);
        SymulatorUAR instancjaTestowa(testPID, testARX);
        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0.0, 0.0, 0.0, 0.54, 0.756, 0.9624, 0.87336, 0.841104, 0.771946, 0.821644,
            0.863453, 0.93272, 0.952656, 0.965421, 0.954525, 0.955787, 0.957472,
            0.969711, 0.978075, 0.985968, 0.987821, 0.989149, 0.989053, 0.990645,
            0.992248, 0.994403, 0.995707, 0.996677, 0.997024, 0.997388
        };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "UAR: Skok 2");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}

void TESTY_ProstyUAR::test_UAR_3_skokJednostkowyPID()
{
    try
    {
        RegulatorPID testPID(1.0, 2.0, 0.2);
        ModelARX testARX({ -0.4 }, { 0.6 }, 1, 0.0);

        SymulatorUAR instancjaTestowa(testPID, testARX);

        constexpr size_t LICZ_ITER = 30;
        std::vector<double> sygWe(LICZ_ITER);
        std::vector<double> spodzSygWy(LICZ_ITER);
        std::vector<double> faktSygWy(LICZ_ITER);

        for (int i = 0; i < LICZ_ITER; i++)
            sygWe[i] = !!i;
        spodzSygWy = { 0.0, 0.0, 1.02, 1.608, 1.1028, 0.41736, 0.546648, 1.20605, 1.43047,
            0.999176, 0.615056, 0.799121, 1.21304, 1.26025, 0.939289, 0.748507,
            0.927166, 1.17292, 1.14155, 0.921616, 0.843258, 0.990018, 1.12577,
            1.068, 0.927024, 0.908125, 1.01702, 1.08484, 1.02618, 0.941508
        };

        for (int i = 0; i < LICZ_ITER; i++)
            faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

        myAssert(spodzSygWy, faktSygWy, "UAR: Skok 3");
    }
    catch (...)
    {
        std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
    }
}


void wykonaj_testy_prowadzacego()
{
    TESTY_ModelARX::wykonaj_testy();
    TESTY_RegulatorPID::wykonaj_testy();
    TESTY_ProstyUAR::wykonaj_testy();
}

#endif

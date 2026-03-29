#include "ARX.h"


ModelARX::ModelARX(std::vector<double> A, std::vector<double> B, int k, double pozsz)
    : _A(A), _B(B), _k(k), _pozsz(pozsz),
    _ograniczenia_wlaczone(true),
    _u_min(-10.0), _u_max(10.0),
    _y_min(-10.0), _y_max(10.0),
    genlos(std::random_device{}()),
    rozklad(0.0, pozsz > 0.0 ? pozsz : 1.0)
{
	updateRozmiar();
}

void ModelARX::updateRozmiar()
{
    /*
     Tu by³ b³¹d, wywala assert gdy jest wiêcej wektorów A ni¿ B przy du¿ym opóŸnieniu transportowym
        size_t n = 0;
        if (_A.size() > _B.size()) {
            n = _A.size();
        }
        else {
            n = _B.size() + _k;
        }
        _u.resize(n, 0.0);
        _y.resize(_A.size(), 0.0);

    Nastêpnie wywo³ywane jest ModelARX::symuluj() w SymulatorUAR
    Program dochodzi do  y += std::inner_product(_B.begin(), _B.end(), _u.begin() + _k, 0.0);
    Zmienna _k wynosi 5. Program próbuje pobraæ  _u.begin() + 5. Poniewa¿ bufor ma w tym momencie rozmiar tylko 3, odwo³anie do 5
        Debug assertion failed
        cannot seek deque iterator after range
    */
    size_t n = std::max(_A.size(), _B.size() + _k);
    _u.resize(n, 0.0);
    _y.resize(_A.size(), 0.0);
}

void ModelARX::setA(std::vector<double>& A)
{
    if (A.empty()) {
        throw std::invalid_argument("wektor A nie moze byc pusty");
    }
    _A = A;
    updateRozmiar();
}

void ModelARX::setB(std::vector<double>& B)
{
    if (B.empty()) {
        throw std::invalid_argument("wektor B nie mo¿e byæ pusty!");
    }
    _B = B;
    updateRozmiar();
}

void ModelARX::setk(int k)
{
    if (k < 1) {
        throw std::invalid_argument("opoznienie ma byc wieksze niz 1");
    }
    _k = k;
    updateRozmiar();
}

void ModelARX::setpozsz(double pozsz)
{
    if (pozsz < 0.0) {
        throw std::invalid_argument("Odchylenie standardowe nie moze byc ujemne");
    }
        _pozsz = pozsz;
        rozklad = std::normal_distribution<double>(0.0, pozsz > 0.0 ? pozsz : 1.0);
}

void ModelARX::setOgraniczeniaSterowania(double min, double max)
{
    if (min > max) {
        throw std::invalid_argument("min > max zamiana wartosci");
    }
    _u_min = min;
    _u_max = max;
}

void ModelARX::setOgraniczeniaRegulowanej(double min, double max)
{
    if (min > max) {
        throw std::invalid_argument("min > max zamiana wartosci");
    }
    _y_min = min;
    _y_max = max;
}

double ModelARX::symuluj(double u)
{
    if (_ograniczenia_wlaczone) {
        if (u > _u_max) {
            u = _u_max;
        }
        else if (u < _u_min) {
            u = _u_min;
        }
    }

    _u_faktyczne = u;

    _u.push_front(u);
    _u.pop_back();

	double y = 0.0; // y bez szumu

    y -= std::inner_product(_A.begin(), _A.end(), _y.begin(), 0.0);

    y += std::inner_product(_B.begin(), _B.end(), _u.begin() + _k, 0.0);

    double szum = (_pozsz > 0.0) ? rozklad(genlos) : 0.0;

    y += szum; //y z szumem

    if (_ograniczenia_wlaczone) {
        if (y > _y_max) {
            y = _y_max;
        }
        else if (y < _y_min) {
            y = _y_min;
        }
    }

    _y.push_front(y);
    if (_y.size() > _A.size())
        _y.pop_back();

    return y;
}

void ModelARX::reset()
{
    _u.assign(_u.size(), 0.0);
    _y.assign(_y.size(), 0.0);
}

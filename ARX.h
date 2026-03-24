#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <deque>
#include <numeric>
#include <random>
#include <stdexcept>

class ModelARX
{
public:
	ModelARX(std::vector<double> A, std::vector<double> B, int k, double pozsz);
	double symuluj(double u);
	void reset();

	std::vector<double> getA() const { return _A; }
	std::vector<double> getB() const { return _B; }
	int getk() const { return _k; }
	double getpozsz() const { return _pozsz; }

	void setA(std::vector<double>& A);

	void setB(std::vector<double>& B);

	void setk(int k);

	void setpozsz(double pozsz);

	bool ograniczenia() const { return _ograniczenia_wlaczone; }
	void setOgraniczenia(bool wlaczone) { _ograniczenia_wlaczone = wlaczone; }

	void setOgraniczeniaSterowania(double min, double max);
	void setOgraniczeniaRegulowanej(double min, double max);

	double getUMin() const { return _u_min; }
	double getUMax() const { return _u_max; }
	double getYMin() const { return _y_min; }
	double getYMax() const { return _y_max; }

    double getUzasymulowane() const { return _u_faktyczne; }

private:
	std::vector<double> _A, _B;
	std::deque<double> _u, _y;
	double _pozsz;
	int _k;

	bool _ograniczenia_wlaczone;
	double _u_min, _u_max;
	double _y_min, _y_max;

	std::mt19937 genlos;
	std::normal_distribution<double> rozklad;

	void updateRozmiar();

    double _u_faktyczne;
};

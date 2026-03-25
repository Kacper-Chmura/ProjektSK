#include "GeneratorSygnalu.h"
#include <cmath>
#include <algorithm>

SygnalSinusoidalny::SygnalSinusoidalny(double amplituda, double okres, double skladowaStala)
	: mAmplituda(amplituda),
	  mOkres(std::max(1e-12, okres)),
	  mSkladowaStala(skladowaStala)
{
}

double SygnalSinusoidalny::generuj(double t)
{
	constexpr double PI = 3.14159265358979323846;
	double omega = 2.0 * PI / mOkres;
	return mAmplituda * std::sin(omega * t) + mSkladowaStala;
}

void SygnalSinusoidalny::ustawParametry(double amplituda, double okres, double skladowaStala)
{
	mAmplituda = amplituda;
	mOkres = std::max(1e-12, okres);
	mSkladowaStala = skladowaStala;
}


SygnalProstokatny::SygnalProstokatny(double amplituda, double okres, double wypelnienie, double skladowaStala)
	: mAmplituda(amplituda),
	  mOkres(std::max(1e-12, okres)),
	  mWypelnienie(std::min(std::max(wypelnienie, 0.0), 1.0)),
	  mSkladowaStala(skladowaStala)
{
}

double SygnalProstokatny::generuj(double t)
{
	if (mOkres <= 0.0) return mSkladowaStala;
	double faza = std::fmod(t, mOkres);
	if (faza < 0.0) faza += mOkres;
	double prog = mWypelnienie * mOkres;
    return (faza < prog) ? (mSkladowaStala + mAmplituda) : (mSkladowaStala);
    // To było źle: return (faza < prog) ? (mSkladowaStala + mAmplituda) : (mSkladowaStala - mAmplituda);
}

void SygnalProstokatny::ustawParametry(double amplituda, double okres, double skladowaStala)
{
	mAmplituda = amplituda;
	mOkres = std::max(1e-12, okres);
	mSkladowaStala = skladowaStala;
}

void SygnalProstokatny::setWypelnienie(double duty)
{
	mWypelnienie = std::min(std::max(duty, 0.0), 1.0);
}

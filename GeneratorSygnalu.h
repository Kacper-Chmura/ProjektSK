#pragma once
#include <cmath>
#include <algorithm>

class GeneratorSygnalu {
public:
	GeneratorSygnalu() = default;
	virtual ~GeneratorSygnalu() = default;

	virtual double generuj(double t) = 0;

	virtual void ustawParametry(double amplituda, double okres, double skladowaStala) = 0;
};

class SygnalSinusoidalny : public GeneratorSygnalu {
public:
	SygnalSinusoidalny(double amplituda = 1.0, double okres = 1.0, double skladowaStala = 0.0);
	double generuj(double t) override;
	void ustawParametry(double amplituda, double okres, double skladowaStala) override;

    double getAmplituda()     const { return mAmplituda; }
    double getOkres()         const { return mOkres; }
    double getSkladowaStala() const { return mSkladowaStala; }

private:
	double mAmplituda;
	double mOkres;
	double mSkladowaStala;
};

class SygnalProstokatny : public GeneratorSygnalu {
public:
	SygnalProstokatny(double amplituda = 1.0, double okres = 1.0, double wypelnienie = 0.5, double skladowaStala = 0.0);
	double generuj(double t) override;
	void ustawParametry(double amplituda, double okres, double skladowaStala) override;
	void setWypelnienie(double duty);

    double getAmplituda()     const { return mAmplituda; }
    double getOkres()         const { return mOkres; }
    double getWypelnienie()   const { return mWypelnienie; }
    double getSkladowaStala() const { return mSkladowaStala; }

private:
	double mAmplituda;
	double mOkres;
    double mWypelnienie;
	double mSkladowaStala;
};
int testgeneratorow();

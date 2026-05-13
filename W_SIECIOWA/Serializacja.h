#ifndef SERIALIZACJA_H
#define SERIALIZACJA_H

#include <QByteArray>
#include <QString>
#include "../ARX.h"
#include "../Reg_PID.h"

namespace MenadzerSymulacjiNS {
    enum class TypGeneratora;
}

struct ParamyGeneratora {
    double amplituda     = 1.0;
    double okres         = 10.0;
    double wypelnienie   = 0.5;
    double skladowaStala = 0.0;
    int    typGeneratora = 0;
};

QByteArray serializePID(const RegulatorPID& pid, quint32 timestamp);
quint32    deserializePID(QByteArray& buf, RegulatorPID& pid);

QByteArray serializeARX(const ModelARX& arx, quint32 timestamp);
quint32    deserializeARX(QByteArray& buf, ModelARX& arx);

QByteArray serializeGenerator(int typGen, const ParamyGeneratora& params);
void       deserializeGenerator(QByteArray& buf, int& outTypGen, ParamyGeneratora& outParams);

QByteArray serializeInfoPolaczenia(int rola, const QString& lokalneIP);
void       deserializeInfoPolaczenia(QByteArray& buf, int& outRola, QString& outIP);

QByteArray serializeSymulacjaRegulator(double czas, double w, double u);
void       deserializeSymulacjaRegulator(QByteArray& buf, double& czas, double& w, double& u);

QByteArray serializeSymulacjaObiekt(double czas, double y);
void       deserializeSymulacjaObiekt(QByteArray& buf, double& czas, double& y);
#endif // SERIALIZACJA_H

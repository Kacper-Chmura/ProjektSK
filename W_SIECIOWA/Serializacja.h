#ifndef SERIALIZACJA_H
#define SERIALIZACJA_H

#include <QByteArray>
#include <QString>
#include "../ARX.h"
#include "../Reg_PID.h"

// forward declaration – unikamy circular include z MenadzerSymulacji
namespace MenadzerSymulacjiNS {
    enum class TypGeneratora;
}

// -------------------------------------------------------------------------
//  Struktura pomocnicza – parametry generatora wartości zadanej
// -------------------------------------------------------------------------
struct ParamyGeneratora {
    double amplituda     = 1.0;
    double okres         = 10.0;
    double wypelnienie   = 0.5;   // tylko prostokąt
    double skladowaStala = 0.0;
    int    typGeneratora = 0;     // 0 = prostokąt, 1 = sinusoida
};

// ---- PID ----------------------------------------------------------------
QByteArray serializePID(const RegulatorPID& pid, quint32 timestamp);
quint32    deserializePID(QByteArray& buf, RegulatorPID& pid);

// ---- ARX ----------------------------------------------------------------
QByteArray serializeARX(const ModelARX& arx, quint32 timestamp);
quint32    deserializeARX(QByteArray& buf, ModelARX& arx);

// ---- Generator ----------------------------------------------------------
QByteArray serializeGenerator(int typGen, const ParamyGeneratora& params);
void       deserializeGenerator(QByteArray& buf, int& outTypGen, ParamyGeneratora& outParams);

// ---- Info połączenia (rola + lokalny IP jako tekst) ---------------------
QByteArray serializeInfoPolaczenia(int rola, const QString& lokalneIP);
void       deserializeInfoPolaczenia(QByteArray& buf, int& outRola, QString& outIP);

#endif // SERIALIZACJA_H

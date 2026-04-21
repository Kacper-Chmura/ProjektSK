#include "Serializacja.h"
#include <QDataStream>
#include <QIODevice>
#include <QVector>
#include <QNetworkInterface>
#include <vector>

// =========================================================================
//  PID
// =========================================================================
QByteArray serializePID(const RegulatorPID& pid, quint32 timestamp) {
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << timestamp
        << pid.getKp() << pid.getTi() << pid.getTd() << pid.getTp()
        << pid.getUMin() << pid.getUMax()
        << (qint32)pid.getLiczCalk();

    return buf;
}

quint32 deserializePID(QByteArray& buf, RegulatorPID& pid) {
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    quint32 timestamp;
    double kp, ti, td, tp, umin, umax;
    qint32 tryb;

    in >> timestamp >> kp >> ti >> td >> tp >> umin >> umax >> tryb;

    pid.ustawParametry(kp, ti, td, umin, umax, tp);
    pid.setLiczCalk(static_cast<RegulatorPID::LiczCalk>(tryb));

    return timestamp;
}

// =========================================================================
//  ARX
// =========================================================================
QByteArray serializeARX(const ModelARX& arx, quint32 timestamp) {
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    QVector<double> qA;
    for (double d : arx.getA()) qA.append(d);

    QVector<double> qB;
    for (double d : arx.getB()) qB.append(d);

    out << timestamp
        << qA << qB
        << (qint32)arx.getk() << arx.getpozsz() << arx.ograniczenia()
        << arx.getUMin() << arx.getUMax()
        << arx.getYMin() << arx.getYMax();

    return buf;
}

quint32 deserializeARX(QByteArray& buf, ModelARX& arx) {
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    quint32 timestamp;
    QVector<double> qA, qB;
    qint32 k;
    double pozsz, umin, umax, ymin, ymax;
    bool ogr;

    in >> timestamp >> qA >> qB >> k >> pozsz >> ogr >> umin >> umax >> ymin >> ymax;

    std::vector<double> stdA(qA.begin(), qA.end());
    std::vector<double> stdB(qB.begin(), qB.end());

    arx.setA(stdA);
    arx.setB(stdB);
    arx.setk(k);
    arx.setpozsz(pozsz);
    arx.setOgraniczenia(ogr);
    arx.setOgraniczeniaSterowania(umin, umax);
    arx.setOgraniczeniaRegulowanej(ymin, ymax);

    return timestamp;
}

// =========================================================================
//  Generator
// =========================================================================
QByteArray serializeGenerator(int typGen, const ParamyGeneratora& p) {
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << (qint32)typGen
        << p.amplituda
        << p.okres
        << p.wypelnienie
        << p.skladowaStala;

    return buf;
}

void deserializeGenerator(QByteArray& buf, int& outTypGen, ParamyGeneratora& p) {
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    qint32 typ;
    in >> typ >> p.amplituda >> p.okres >> p.wypelnienie >> p.skladowaStala;
    outTypGen = (int)typ;
    p.typGeneratora = outTypGen;
}

// =========================================================================
//  Info połączenia
// =========================================================================
QByteArray serializeInfoPolaczenia(int rola, const QString& lokalneIP) {
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    out << (qint32)rola << lokalneIP;
    return buf;
}

void deserializeInfoPolaczenia(QByteArray& buf, int& outRola, QString& outIP) {
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    qint32 rola;
    in >> rola >> outIP;
    outRola = (int)rola;
}

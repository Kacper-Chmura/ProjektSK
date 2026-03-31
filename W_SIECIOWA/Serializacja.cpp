#include "Serializacja.h"
#include <QDataStream>
#include <QIODevice>
#include <QVector>
#include <vector>

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

QByteArray serializeARX(const ModelARX& arx, quint32 timestamp) {
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);

    QVector<double> qA;
    for(double d : arx.getA()) qA.append(d);

    QVector<double> qB;
    for(double d : arx.getB()) qB.append(d);

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

    std::vector<double> stdA;
    stdA.reserve(qA.size());
    for(double d : qA) stdA.push_back(d);

    std::vector<double> stdB;
    stdB.reserve(qB.size());
    for(double d : qB) stdB.push_back(d);

    arx.setA(stdA);
    arx.setB(stdB);

    arx.setk(k);
    arx.setpozsz(pozsz);
    arx.setOgraniczenia(ogr);
    arx.setOgraniczeniaSterowania(umin, umax);
    arx.setOgraniczeniaRegulowanej(ymin, ymax);

    return timestamp;
}

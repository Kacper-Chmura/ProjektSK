#include "Serializacja.h"
#include "qabstractsocket.h"
#include <QDataStream>
#include <QVector>

QByteArray serializePID(const RegulatorPID& pid, quint32 timestamp) {
    QByteArray buf;
    QDataStream out(&buf, QIODevice::WriteOnly);

    out << timestamp
        << pid.getKp() << pid.getTi() << pid.getTd() << pid.getTp()
        << pid.getUMin() << pid.getUMax()
        << (qint32)pid.getLiczCalk();

    return buf;
}

quint32 deserializePID(QByteArray& buf, RegulatorPID& pid) {
    QDataStream in(&buf, QIODevice::ReadOnly);

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

    QVector<double> qA(arx.getA().begin(), arx.getA().end());
    QVector<double> qB(arx.getB().begin(), arx.getB().end());

    out << timestamp
        << qA << qB
        << (qint32)arx.getk() << arx.getpozsz() << arx.ograniczenia()
        << arx.getUMin() << arx.getUMax()
        << arx.getYMin() << arx.getYMax();

    return buf;
}

quint32 deserializeARX(QByteArray& buf, ModelARX& arx) {
    QDataStream in(&buf, QIODevice::ReadOnly);

    quint32 timestamp;
    QVector<double> qA, qB;
    qint32 k;
    double pozsz;
    bool ogr;
    double umin, umax, ymin, ymax;

    in >> timestamp >> qA >> qB >> k >> pozsz >> ogr >> umin >> umax >> ymin >> ymax;

    std::vector<double> sA(qA.begin(), qA.end());
    std::vector<double> sB(qB.begin(), qB.end());

    arx.setA(sA);
    arx.setB(sB);
    arx.setk(k);
    arx.setpozsz(pozsz);
    arx.setOgraniczenia(ogr);
    arx.setOgraniczeniaSterowania(umin, umax);
    arx.setOgraniczeniaRegulowanej(ymin, ymax);

    return timestamp;
}

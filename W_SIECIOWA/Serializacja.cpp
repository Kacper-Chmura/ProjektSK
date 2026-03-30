#include "Serializacja.h"
#include "qabstractsocket.h"
#include <QDataStream>
#include <QVector>
#include <iostream>

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
    out.setVersion(QDataStream::Qt_6_0); // DODAJ TĘ LINIĘ

    QVector<double> qA = QVector<double>(arx.getA().begin(), arx.getA().end());
    QVector<double> qB = QVector<double>(arx.getB().begin(), arx.getB().end());

    out << timestamp << qA << qB << (qint32)arx.getk()
        << arx.getpozsz() << arx.ograniczenia()
        << arx.getUMin() << arx.getUMax()
        << arx.getYMin() << arx.getYMax();

    return buf;
}

quint32 deserializeARX(QByteArray& buf, ModelARX& arx) {
    if (buf.isEmpty()) return 0;

    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    quint32 timestamp;
    QVector<double> qA, qB;
    qint32 k;
    double pozsz;
    bool ogr;
    double umin, umax, ymin, ymax;

    in >> timestamp >> qA >> qB >> k >> pozsz >> ogr >> umin >> umax >> ymin >> ymax;

    // DEBUG: Sprawdź w konsoli, co widzi program
    std::cout << "DEBUG: Rozmiar qA = " << qA.size() << " Rozmiar qB = " << qB.size() << std::endl;

    if (qA.size() > 1000 || qB.size() > 1000) return 0;

    // Bezpieczne przepisywanie (bez iteratorów)
    std::vector<double> sA, sB;
    for(int i = 0; i < qA.size(); ++i) sA.push_back(qA.at(i));
    for(int i = 0; i < qB.size(); ++i) sB.push_back(qB.at(i));

    arx.setA(sA);
    arx.setB(sB);
    arx.setk(k);
    arx.setpozsz(pozsz);
    arx.setOgraniczenia(ogr);
    arx.setOgraniczeniaSterowania(umin, umax);
    arx.setOgraniczeniaRegulowanej(ymin, ymax);

    return timestamp;
}

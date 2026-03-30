#ifndef SERIALIZACJA_H
#define SERIALIZACJA_H

#include <QByteArray>
#include "ARX.h"
#include "Reg_PID.h"

QByteArray serializePID(const RegulatorPID& pid, quint32 timestamp);
quint32 deserializePID(QByteArray& buf, RegulatorPID& pid);

QByteArray serializeARX(const ModelARX& arx, quint32 timestamp);
quint32 deserializeARX(QByteArray& buf, ModelARX& arx);

#endif // SERIALIZACJA_H

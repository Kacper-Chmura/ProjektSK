#include "myTCPclient.h"
#include <QDataStream>

MyTCPClient::MyTCPClient(QObject *parent) : QObject(parent), m_socket(this)
{
    connect(&m_socket, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(slot_readyRead()));
}

void MyTCPClient::connectTo(QString address, int port) {
    m_ipAddress = address;
    m_port = port;
    m_socket.connectToHost(m_ipAddress, port);
}

void MyTCPClient::disconnectFrom() {
    m_socket.close();
}

void MyTCPClient::wyslijRamke(quint8 typ, const QByteArray& payload) {
    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);

    out << (quint8)0xAA;               // STX
    out << typ;                        // Typ wiadomości
    out << (quint16)payload.size();    // Długość danych

    frame.append(payload);             // Dane

    quint16 crc = 0;
    for (char b : frame) crc += (quint8)b;
    out << crc;                        // CRC

    m_socket.write(frame);
}

void MyTCPClient::slot_connected() {
    emit connected(m_ipAddress, m_port);
}

void MyTCPClient::slot_readyRead() {
    QByteArray data = m_socket.readAll();
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0);

    quint8 stx, typ;
    quint16 rozmiar;

    in >> stx;
    if (stx != 0xAA) return;

    in >> typ >> rozmiar;

    // Zabezpieczenie przed niepełną ramką (fragmentacja TCP)
    if (data.size() < (4 + rozmiar)) return;

    QByteArray payload = data.mid(4, rozmiar);
    emit nowaRamka(typ, payload);
}

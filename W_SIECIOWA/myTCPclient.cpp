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

void MyTCPClient::wyslijRamke(int typ, int timestamp, const QByteArray& payload) {
    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);

    out << typ << (int)payload.size() << timestamp;
    frame.append(payload);

    int crc = 0;
    for(char b : frame) crc ^= b;
    out << crc;

    m_socket.write(frame);
}

void MyTCPClient::slot_connected() {
    emit connected(m_ipAddress, m_port);
}

void MyTCPClient::slot_readyRead() {
    QByteArray data = m_socket.readAll();
    QDataStream in(&data, QIODevice::ReadOnly);

    int typ, rozmiar, timestamp;
    in >> typ >> rozmiar >> timestamp;

    QByteArray payload = data.mid(3 * sizeof(int), rozmiar);

    emit nowaRamka(typ, payload);
}

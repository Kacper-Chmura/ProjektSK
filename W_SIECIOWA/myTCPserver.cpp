#include "myTCPserver.h"
#include <QDataStream>
#include <iostream>

MyTCPServer::MyTCPServer(QObject *parent) : QObject(parent), m_server(this)
{
    connect(&m_server, SIGNAL(newConnection()), this, SLOT(slot_new_client()));
}

bool MyTCPServer::startListening(int port) {
    m_port = port;
    m_isListening = m_server.listen(QHostAddress::Any, port);
    return m_isListening;
}

void MyTCPServer::stopListening() {
    m_server.close();
    m_isListening = false;
}

int MyTCPServer::getNumClients() {
    return m_clients.length();
}

void MyTCPServer::wyslijRamke(quint8 typ, const QByteArray& payload, int numCli) {
    if(numCli < 0 || numCli >= m_clients.length()) return;

    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);
    // KLUCZOWE: Wersja musi być identyczna wszędzie!
    out.setVersion(QDataStream::Qt_6_0);

    // 1. Nagłówek (4 bajty)
    out << (quint8)0xAA;
    out << typ;
    out << (quint16)payload.size();

    // 2. Dane - informujemy strumień o zapisie surowych bajtów
    out.writeRawData(payload.constData(), payload.size());

    // 3. Suma kontrolna - teraz zostanie dopisana PO danych
    quint16 crc = 0;
    for (char b : frame) crc += (quint8)b;
    out << crc;

    m_clients.at(numCli)->write(frame);
}
int MyTCPServer::getClinetID() {
    QTcpSocket *client = static_cast<QTcpSocket*>(QObject::sender());
    return m_clients.indexOf(client);
}

void MyTCPServer::slot_new_client() {
    QTcpSocket *client = m_server.nextPendingConnection();
    m_clients.push_back(client);

    connect(client, SIGNAL(disconnected()), this, SLOT(slot_client_disconnetcted()));
    connect(client, SIGNAL(readyRead()), this, SLOT(slot_newMsg()));

    emit newClientConnected(client->peerAddress().toString());
}

void MyTCPServer::slot_client_disconnetcted() {
    int idx = getClinetID();
    m_clients.removeAt(idx);
    emit clientDisconnetced(idx);
}
void MyTCPServer::slot_newMsg() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_6_0); // DODAJ TO

    quint8 stx = 0;
    in >> stx;
    if (stx != 0xAA) return;

    quint8 typ;
    quint16 rozmiar;
    in >> typ >> rozmiar;

    QByteArray payload = data.mid(4, rozmiar);
    emit nowaRamkaOd(typ, payload, m_clients.indexOf(socket));
}

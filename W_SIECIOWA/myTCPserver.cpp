#include "myTCPserver.h"
#include <QDataStream>

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

void MyTCPServer::wyslijRamke(int typ, int timestamp, const QByteArray& payload, int numCli) {
    if(numCli >= m_clients.length()) return;

    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);

    out << typ << (int)payload.size() << timestamp;
    frame.append(payload);

    int crc = 0;
    for(char b : frame) crc ^= b;
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
    int idx = getClinetID();
    QByteArray data = m_clients.at(idx)->readAll();

    QDataStream in(&data, QIODevice::ReadOnly);
    int typ, rozmiar, timestamp;
    in >> typ >> rozmiar >> timestamp;

    QByteArray payload = data.mid(3 * sizeof(int), rozmiar);

    emit nowaRamkaOd(typ, payload, idx);
}

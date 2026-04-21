#include "myTCPserver.h"
#include <QDataStream>

static const int NAGLOWEK_ROZMIAR = 4;

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
    for (QTcpSocket* s : m_clients) {
        s->disconnectFromHost();
        s->deleteLater();
    }
    m_clients.clear();
    m_bufory.clear();
    m_server.close();
    m_isListening = false;
}

int MyTCPServer::getNumClients() {
    return m_clients.length();
}

void MyTCPServer::wyslijRamke(quint8 typ, const QByteArray& payload, int numCli) {
    if (numCli < 0 || numCli >= m_clients.length()) return;

    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    out << (quint8)0xAA;
    out << typ;
    out << (quint16)payload.size();

    frame.append(payload);

    quint16 crc = 0;
    for (unsigned char b : frame) crc += b;
    QDataStream crcOut(&frame, QIODevice::Append);
    crcOut.setByteOrder(QDataStream::BigEndian);
    crcOut << crc;

    m_clients.at(numCli)->write(frame);
}

int MyTCPServer::getClientID(QTcpSocket* socket) {
    return m_clients.indexOf(socket);
}

void MyTCPServer::slot_new_client() {
    QTcpSocket *client = m_server.nextPendingConnection();
    m_clients.push_back(client);
    m_bufory[client] = QByteArray();

    connect(client, SIGNAL(disconnected()), this, SLOT(slot_client_disconnetcted()));
    connect(client, SIGNAL(readyRead()),    this, SLOT(slot_newMsg()));

    emit newClientConnected(client->peerAddress().toString());
}

void MyTCPServer::slot_client_disconnetcted() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    int idx = getClientID(socket);
    m_clients.removeAt(idx);
    m_bufory.remove(socket);
    socket->deleteLater();
    emit clientDisconnetced(idx);
}

void MyTCPServer::slot_newMsg() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    m_bufory[socket].append(socket->readAll());
    przetorzBufor(socket);
}

void MyTCPServer::przetorzBufor(QTcpSocket* socket) {
    QByteArray& buf = m_bufory[socket];
    int numCli = getClientID(socket);

    while (true) {
        if (buf.size() < NAGLOWEK_ROZMIAR) break;

        if ((quint8)buf[0] != 0xAA) {
            int idx = buf.indexOf((char)0xAA, 1);
            if (idx < 0) { buf.clear(); break; }
            buf.remove(0, idx);
            continue;
        }

        quint8  typ     = (quint8)buf[1];
        quint16 rozmiar = ((quint8)buf[2] << 8) | (quint8)buf[3];

        int calkowitaRozmiar = NAGLOWEK_ROZMIAR + rozmiar + 2;

        if (buf.size() < calkowitaRozmiar) break;

        quint16 crcObliczone = 0;
        for (int i = 0; i < NAGLOWEK_ROZMIAR + rozmiar; ++i)
            crcObliczone += (quint8)buf[i];

        quint16 crcOdebrane = ((quint8)buf[NAGLOWEK_ROZMIAR + rozmiar] << 8)
                            |  (quint8)buf[NAGLOWEK_ROZMIAR + rozmiar + 1];

        if (crcObliczone == crcOdebrane) {
            QByteArray payload = buf.mid(NAGLOWEK_ROZMIAR, rozmiar);
            emit nowaRamkaOd(typ, payload, numCli);
        }

        buf.remove(0, calkowitaRozmiar);
    }
}

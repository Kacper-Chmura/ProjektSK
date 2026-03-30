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

void MyTCPServer::wyslijRamke(quint8 typ, const QByteArray& payload, int numCli) {
    if(numCli >= m_clients.length()) return;

    QByteArray frame;
    QDataStream out(&frame, QIODevice::WriteOnly);

    // Zgodnie z wytycznymi z obrazka:
    out << (quint8)0xAA;               // STX (1 bajt)
    out << typ;                        // Typ (1 bajt)
    out << (quint16)payload.size();    // Długość (2 bajty)

    frame.append(payload);             // Dane (zawierają już timestamp z Serializacja.h)

    quint16 crc = 0;
    for (char b : frame) crc += (quint8)b;
    out << crc;                        // CRC (2 bajty)

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

    // ZMIENNE O POPRAWNYCH ROZMIARACH (zgodnie z tabelą)
    quint8 stx = 0;
    quint8 typ = 0;
    quint16 rozmiar = 0;

    // 1. Odczytujemy bajt po bajcie zgodnie ze strukturą
    in >> stx;

    // Sprawdzamy czy to nasza ramka
    if (stx != 0xAA) {
        qDebug() << "[SERWER] Odrzucono ramkę: Błędny STX!";
        return;
    }

    // 2. Odczytujemy typ (1 bajt) i rozmiar (2 bajty)
    in >> typ >> rozmiar;

    // 3. Wycinamy właściwe dane (payload)
    // Nagłówek (STX + Typ + Rozmiar) zajmuje 1 + 1 + 2 = 4 bajty
    QByteArray payload = data.mid(4, rozmiar);

    // Pobieramy numer klienta (jeśli go przechowujesz w mapie/liście)
    int numCli = m_clients.indexOf(socket);

    // Emitujemy sygnał z poprawnym typem (teraz 'typ' będzie wynosił 1)
    emit nowaRamkaOd(typ, payload, numCli);
}

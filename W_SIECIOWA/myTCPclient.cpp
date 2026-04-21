#include "myTCPclient.h"
#include <QDataStream>

// Rozmiar nagłówka ramki: STX(1) + typ(1) + rozmiar(2) = 4 bajty
static const int NAGLOWEK_ROZMIAR = 4;

MyTCPClient::MyTCPClient(QObject *parent) : QObject(parent), m_socket(this)
{
    connect(&m_socket, SIGNAL(connected()),    this, SLOT(slot_connected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(&m_socket, SIGNAL(readyRead()),    this, SLOT(slot_readyRead()));
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
    out.setByteOrder(QDataStream::BigEndian);

    out << (quint8)0xAA;                    // STX
    out << typ;                             // Typ wiadomości
    out << (quint16)payload.size();         // Długość danych

    frame.append(payload);                  // Dane

    // CRC – suma wszystkich bajtów ramki (bez samego CRC)
    quint16 crc = 0;
    for (unsigned char b : frame) crc += b;
    QDataStream crcOut(&frame, QIODevice::Append);
    crcOut.setByteOrder(QDataStream::BigEndian);
    crcOut << crc;

    m_socket.write(frame);
}

void MyTCPClient::slot_connected() {
    emit connected(m_ipAddress, m_port);
}

// ---------------------------------------------------------------------------
//  Odbiór z buforowaniem – poprawna obsługa fragmentacji TCP
// ---------------------------------------------------------------------------
void MyTCPClient::slot_readyRead() {
    m_buforOdbioru.append(m_socket.readAll());
    przetorzBufor();
}

void MyTCPClient::przetorzBufor() {
    // Pętla: przetwarzamy tyle pełnych ramek ile jest w buforze
    while (true) {
        // Za mało danych nawet na nagłówek
        if (m_buforOdbioru.size() < NAGLOWEK_ROZMIAR)
            break;

        // Sprawdź STX
        if ((quint8)m_buforOdbioru[0] != 0xAA) {
            // Zsynchronizuj – szukaj następnego 0xAA
            int idx = m_buforOdbioru.indexOf((char)0xAA, 1);
            if (idx < 0) { m_buforOdbioru.clear(); break; }
            m_buforOdbioru.remove(0, idx);
            continue;
        }

        quint8  typ     = (quint8)m_buforOdbioru[1];
        quint16 rozmiar = ((quint8)m_buforOdbioru[2] << 8) | (quint8)m_buforOdbioru[3];

        int calkowitaRozmiar = NAGLOWEK_ROZMIAR + rozmiar + 2; // +2 na CRC

        // Jeszcze nie mamy całej ramki
        if (m_buforOdbioru.size() < calkowitaRozmiar)
            break;

        // Weryfikacja CRC
        quint16 crcObliczone = 0;
        for (int i = 0; i < NAGLOWEK_ROZMIAR + rozmiar; ++i)
            crcObliczone += (quint8)m_buforOdbioru[i];

        quint16 crcOdebrane = ((quint8)m_buforOdbioru[NAGLOWEK_ROZMIAR + rozmiar] << 8)
                            |  (quint8)m_buforOdbioru[NAGLOWEK_ROZMIAR + rozmiar + 1];

        if (crcObliczone == crcOdebrane) {
            QByteArray payload = m_buforOdbioru.mid(NAGLOWEK_ROZMIAR, rozmiar);
            emit nowaRamka(typ, payload);
        }
        // (błąd CRC – ramkę odrzucamy, ale usuwamy ją z bufora)

        m_buforOdbioru.remove(0, calkowitaRozmiar);
    }
}

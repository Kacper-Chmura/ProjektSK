#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class MyTCPClient : public QObject
{
    Q_OBJECT
public:
    explicit MyTCPClient(QObject *parent = nullptr);

    void connectTo(QString address, int port);
    void disconnectFrom();
    bool isConnected() { return m_socket.isOpen(); }

    void wyslijRamke(quint8 typ, const QByteArray& payload);

signals:
    void connected(QString adr, int port);
    void disconnected();
    void nowaRamka(int typ, QByteArray payload);

private slots:
    void slot_connected();
    void slot_readyRead();

private:
    void przetorzBufor();

    QTcpSocket m_socket;
    QString    m_ipAddress = "127.0.0.1";
    int        m_port      = 12345;

    QByteArray m_buforOdbioru;
};

#endif // MYTCPCLIENT_H

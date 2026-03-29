#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QByteArray>

class MyTCPServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTCPServer(QObject *parent = nullptr);

    bool startListening(int port);
    void stopListening();
    bool isListening() { return m_isListening; }
    int getNumClients();

    // Nowa metoda wysyłająca ramkę
    void wyslijRamke(int typ, int timestamp, const QByteArray& payload, int numCli);

signals:
    void newClientConnected(QString adr);
    void clientDisconnetced(int num);
    // Nowy sygnał emitowany po odebraniu i zdekodowaniu ramki
    void nowaRamkaOd(int typ, QByteArray payload, int numCli);

private slots:
    void slot_new_client();
    void slot_client_disconnetcted();
    void slot_newMsg();

private:
    int getClinetID();

    bool m_isListening = false;
    int m_port = 12345;
    QTcpServer m_server;
    QVector<QTcpSocket*> m_clients;
};

#endif // MYTCPSERVER_H

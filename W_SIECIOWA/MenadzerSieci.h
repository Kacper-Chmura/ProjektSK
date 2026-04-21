#pragma once

#include <QObject>
#include <QString>
#include "myTCPclient.h"
#include "myTCPserver.h"

class MenadzerSymulacji;

enum class TypRamki : quint8 {
    PID           = 0x01,
    ARX           = 0x02,
    Generator     = 0x03,
    InfoPolaczenia = 0x04,
    Potwierdzenie = 0x05,
};

enum class RolaSieciowa {
    Brak,
    Regulator,
    Obiekt,
};

class MenadzerSieci : public QObject
{
    Q_OBJECT

public:
    explicit MenadzerSieci(MenadzerSymulacji* manager, QObject* parent = nullptr);
    ~MenadzerSieci() override = default;

    bool startujSerwer(int port);
    void polaczJakoKlient(const QString& adres, int port);
    void rozlacz();

    bool czyPolaczony() const { return _polaczony; }
    RolaSieciowa getRole() const { return _rola; }
    void setRole(RolaSieciowa r) { _rola = r; }

    void wyslijKonfiguracjePID();
    void wyslijKonfiguracjeARX();
    void wyslijKonfiguracjeGeneratora();
    void wyslijPelnaKonfiguracje();

    QString getZdalneIP() const { return _zdalneIP; }
    int     getZdalnyPort() const { return _zdalnyPort; }
    bool    czyJestSerwerem() const { return _czyJestSerwer; }

signals:
    void polaczonySygnal(QString zdalneIP, int zdalnyPort, bool jakoSerwer);
    void rozlaczenieZewnetrzne();
    void konfiguracjaOdebrana(TypRamki typ);

private slots:
    void onKlientPolaczony(QString adres, int port);
    void onKlientRozlaczony();
    void onNowaRamkaKlient(int typ, QByteArray payload);

    void onNowyKlientSerwer(QString adres);
    void onKlientSerwRozlaczony(int num);
    void onNowaRamkaSerwer(int typ, QByteArray payload, int numCli);

private:
    void obsluzRamke(TypRamki typ, QByteArray payload);
    void wyslijRamke(quint8 typ, const QByteArray& payload);

    MenadzerSymulacji* _manager;

    MyTCPClient* _klient;
    MyTCPServer* _serwer;

    bool          _polaczony    = false;
    bool          _czyJestSerwer = false;
    QString       _zdalneIP;
    int           _zdalnyPort   = 0;
    RolaSieciowa  _rola         = RolaSieciowa::Brak;

    QByteArray _buforOdbioru;
};

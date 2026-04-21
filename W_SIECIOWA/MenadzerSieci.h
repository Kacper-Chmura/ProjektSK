#pragma once

#include <QObject>
#include <QString>
#include "myTCPclient.h"
#include "myTCPserver.h"

class MenadzerSymulacji;

// -------------------------------------------------------------------------
//  Typy ramek protokołu
// -------------------------------------------------------------------------
enum class TypRamki : quint8 {
    PID           = 0x01,   // konfiguracja regulatora PID
    ARX           = 0x02,   // konfiguracja modelu ARX
    Generator     = 0x03,   // konfiguracja generatora wartości zadanej
    InfoPolaczenia = 0x04,  // informacje wymieniane przy nawiązaniu (IP, rola)
    Potwierdzenie = 0x05,   // ACK – odebrano konfigurację
};

// -------------------------------------------------------------------------
//  Role instancji w trybie sieciowym
// -------------------------------------------------------------------------
enum class RolaSieciowa {
    Brak,       // tryb stacjonarny
    Regulator,  // zarządza generatorem, PID, taktowaniem – wysyła u
    Obiekt,     // symuluje ARX – odbiera u, odsyła y
};

// -------------------------------------------------------------------------
//  MenadzerSieci – warstwa sieciowa
// -------------------------------------------------------------------------
class MenadzerSieci : public QObject
{
    Q_OBJECT

public:
    explicit MenadzerSieci(MenadzerSymulacji* manager, QObject* parent = nullptr);
    ~MenadzerSieci() override = default;

    // ---- nawiązywanie / zrywanie połączenia ----
    bool startujSerwer(int port);
    void polaczJakoKlient(const QString& adres, int port);
    void rozlacz();

    bool czyPolaczony() const { return _polaczony; }
    RolaSieciowa getRole() const { return _rola; }
    void setRole(RolaSieciowa r) { _rola = r; }

    // ---- wysyłanie konfiguracji UAR ----
    void wyslijKonfiguracjePID();
    void wyslijKonfiguracjeARX();
    void wyslijKonfiguracjeGeneratora();
    void wyslijPelnaKonfiguracje();  // PID + ARX + Generator

    // ---- informacje o połączeniu ----
    QString getZdalneIP() const { return _zdalneIP; }
    int     getZdalnyPort() const { return _zdalnyPort; }
    bool    czyJestSerwerem() const { return _czyJestSerwer; }

signals:
    void polaczonySygnal(QString zdalneIP, int zdalnyPort, bool jakoSerwer);
    void rozlaczenieZewnetrzne();          // połączenie zerwane z zewnątrz
    void konfiguracjaOdebrana(TypRamki typ); // sygnał po odebraniu i zastosowaniu konfigu

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

    // bufor do obsługi fragmentacji TCP
    QByteArray _buforOdbioru;
};

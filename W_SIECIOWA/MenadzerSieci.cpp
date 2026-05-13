#include "MenadzerSieci.h"
#include "Serializacja.h"
#include "../MenadzerSymulacji.h"
#include "ZarzadzanieCzasem.h"
#include <QDataStream>
#include <QNetworkInterface>

MenadzerSieci::MenadzerSieci(MenadzerSymulacji* manager, QObject* parent)
    : QObject(parent), _manager(manager)
{
    _klient = new MyTCPClient(this);
    _serwer = new MyTCPServer(this);

    connect(_klient, &MyTCPClient::connected,    this, &MenadzerSieci::onKlientPolaczony);
    connect(_klient, &MyTCPClient::disconnected, this, &MenadzerSieci::onKlientRozlaczony);
    connect(_klient, &MyTCPClient::nowaRamka,    this, &MenadzerSieci::onNowaRamkaKlient);

    connect(_serwer, &MyTCPServer::newClientConnected, this, &MenadzerSieci::onNowyKlientSerwer);
    connect(_serwer, &MyTCPServer::clientDisconnetced, this, &MenadzerSieci::onKlientSerwRozlaczony);
    connect(_serwer, &MyTCPServer::nowaRamkaOd,        this, &MenadzerSieci::onNowaRamkaSerwer);

    connect(_manager, &MenadzerSymulacji::wyslijRamkeRegulatora, this, [this](double czas, double w, double u){
        QByteArray buf = serializeSymulacjaRegulator(czas, w, u);
        wyslijRamke(static_cast<quint8>(TypRamki::SymulacjaRegulator), buf);
    });

    connect(_manager, &MenadzerSymulacji::wyslijRamkeObiektu, this, [this](double czas, double y){
        QByteArray buf = serializeSymulacjaObiekt(czas, y);
        wyslijRamke(static_cast<quint8>(TypRamki::SymulacjaObiekt), buf);
    });
}

bool MenadzerSieci::startujSerwer(int port)
{
    _czyJestSerwer = true;
    return _serwer->startListening(port);
}

void MenadzerSieci::polaczJakoKlient(const QString& adres, int port)
{
    _czyJestSerwer = false;
    _zdalnyPort = port;
    _klient->connectTo(adres, port);
}

void MenadzerSieci::rozlacz()
{
    if (_czyJestSerwer) {
        _serwer->stopListening();
    } else {
        _klient->disconnectFrom();
    }
    _polaczony = false;
    _zdalneIP.clear();
}

void MenadzerSieci::wyslijRamke(quint8 typ, const QByteArray& payload)
{
    if (!_polaczony) return;

    if (_czyJestSerwer) {
        if (_serwer->getNumClients() > 0)
            _serwer->wyslijRamke(typ, payload, 0);
    } else {
        _klient->wyslijRamke(typ, payload);
    }
}

void MenadzerSieci::wyslijKonfiguracjePID()
{
    QByteArray data = serializePID(*_manager->getPID(), 0);
    wyslijRamke(static_cast<quint8>(TypRamki::PID), data);
}

void MenadzerSieci::wyslijKonfiguracjeARX()
{
    QByteArray data = serializeARX(*_manager->getARX(), 0);
    wyslijRamke(static_cast<quint8>(TypRamki::ARX), data);
}

void MenadzerSieci::wyslijKonfiguracjeGeneratora()
{
    ParamyGeneratora pg;
    pg.amplituda     = _manager->getGenAmplituda();
    pg.okres         = _manager->getGenOkres();
    pg.wypelnienie   = _manager->getGenWypelnienie();
    pg.skladowaStala = _manager->getGenSkladowaStala();
    pg.typGeneratora = static_cast<int>(_manager->getTypGeneratora());

    QByteArray data = serializeGenerator(pg.typGeneratora, pg);
    wyslijRamke(static_cast<quint8>(TypRamki::Generator), data);
}

void MenadzerSieci::wyslijPelnaKonfiguracje()
{
    wyslijKonfiguracjePID();
    wyslijKonfiguracjeARX();
    wyslijKonfiguracjeGeneratora();
}

static QString pobierzLokalneIP()
{
    for (const QNetworkInterface& iface : QNetworkInterface::allInterfaces()) {
        if (iface.flags() & QNetworkInterface::IsLoopBack) continue;
        for (const QNetworkAddressEntry& entry : iface.addressEntries()) {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                return entry.ip().toString();
        }
    }
    return "127.0.0.1";
}

void MenadzerSieci::onKlientPolaczony(QString adres, int port)
{
    _polaczony  = true;
    _zdalneIP   = adres;
    _zdalnyPort = port;

    QString lokalneIP = pobierzLokalneIP();
    QByteArray info = serializeInfoPolaczenia(static_cast<int>(_rola), lokalneIP);
    _klient->wyslijRamke(static_cast<quint8>(TypRamki::InfoPolaczenia), info);

    wyslijPelnaKonfiguracje();

    emit polaczonySygnal(adres, port, false);
}

void MenadzerSieci::onKlientRozlaczony()
{
    if (_polaczony) {
        _polaczony = false;
        _zdalneIP.clear();
        emit rozlaczenieZewnetrzne();
    }
}

void MenadzerSieci::onNowaRamkaKlient(int typ, QByteArray payload)
{
    obsluzRamke(static_cast<TypRamki>(typ), payload);
}

void MenadzerSieci::onNowyKlientSerwer(QString adres)
{
    _polaczony = true;
    _zdalneIP  = adres;

    QString lokalneIP = pobierzLokalneIP();
    QByteArray info = serializeInfoPolaczenia(static_cast<int>(_rola), lokalneIP);
    _serwer->wyslijRamke(static_cast<quint8>(TypRamki::InfoPolaczenia), info, 0);

    wyslijPelnaKonfiguracje();

    emit polaczonySygnal(adres, _zdalnyPort, true);
}

void MenadzerSieci::onKlientSerwRozlaczony(int)
{
    if (_polaczony) {
        _polaczony = false;
        _zdalneIP.clear();
        emit rozlaczenieZewnetrzne();
    }
}

void MenadzerSieci::onNowaRamkaSerwer(int typ, QByteArray payload, int)
{
    obsluzRamke(static_cast<TypRamki>(typ), payload);
}

void MenadzerSieci::obsluzRamke(TypRamki typ, QByteArray payload)
{
    switch (typ) {
    case TypRamki::PID: {
        RegulatorPID* pid = _manager->getPID();
        deserializePID(payload, *pid);

        _manager->getZarzadzanieCzasem()->setInterwalMs(pid->getTp() * 1000.0);

        emit konfiguracjaOdebrana(TypRamki::PID);
        break;
    }
    case TypRamki::ARX: {
        ModelARX* arx = _manager->getARX();
        deserializeARX(payload, *arx);
        emit konfiguracjaOdebrana(TypRamki::ARX);
        break;
    }
    case TypRamki::Generator: {
        int typGen;
        ParamyGeneratora pg;
        deserializeGenerator(payload, typGen, pg);

        auto tg = static_cast<MenadzerSymulacji::TypGeneratora>(typGen);
        _manager->setTypGeneratora(tg);

        if (tg == MenadzerSymulacji::TypGeneratora::Sinusoidalny) {
            _manager->setParametryGeneratoraSinusoidalnego(pg.amplituda, pg.okres, pg.skladowaStala);
        } else {
            _manager->setParametryGeneratoraProstokatnego(pg.amplituda, pg.okres, pg.wypelnienie, pg.skladowaStala);
        }

        emit konfiguracjaOdebrana(TypRamki::Generator);
        break;
    }
    case TypRamki::InfoPolaczenia: {
        int rola;
        QString ip;
        deserializeInfoPolaczenia(payload, rola, ip);
        _zdalneIP = ip;
        emit konfiguracjaOdebrana(TypRamki::InfoPolaczenia);
        break;
    }
    case TypRamki::SymulacjaRegulator: {
        double czas, w, u;
        deserializeSymulacjaRegulator(payload, czas, w, u);
        _manager->aktualizujZSieciRegulator(czas, w, u);
        break;
    }
    case TypRamki::SymulacjaObiekt: {
        double czas, y;
        deserializeSymulacjaObiekt(payload, czas, y);
        _manager->aktualizujZSieciObiekt(y);
        break;
    }
    default:
        break;
    }
}

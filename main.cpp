#include "W_SIECIOWA/Serializacja.h"
#include "W_SIECIOWA/myTCPclient.h"
#include "W_SIECIOWA/myTCPserver.h"
#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

#include "ARX.h"
#include "Reg_PID.h"
#include "GeneratorSygnalu.h"
#include "MenadzerSymulacji.h"
#include "SymulatorUAR.h"
#include "TestUtils.h"

int total_tests = 0;
int passed_tests = 0;

void wykonaj_testy_prowadzacego();

void check(const std::string& msg, double spodz, double fakt) {
    total_tests++;
    if (std::abs(spodz - fakt) <= 1e-3) {
        passed_tests++;
    } else {
        std::cerr << "[FAIL] " << msg << " | Spodziewano: " << spodz << " | Jest: " << fakt << std::endl;
    }
}

void wykonaj_moje_testy() {
    std::cout << "10 testów" << std::endl;

    ModelARX arx1({-0.5}, {1.0}, 1, 0);
    arx1.setOgraniczenia(true);
    arx1.setOgraniczeniaSterowania(-1.0, 1.0);
    arx1.symuluj(10.0);
    check("1. ARX: Saturacja wejscia", 1.0, arx1.symuluj(10.0));


    ModelARX arx2({-0.5}, {10.0}, 1, 0);
    arx2.setOgraniczenia(true);
    arx2.setOgraniczeniaRegulowanej(-5.0, 5.0);
    arx2.symuluj(1.0);
    check("2. ARX: Saturacja wyjscia", 5.0, arx2.symuluj(1.0));


    arx1.reset();
    check("3. ARX: Reset stanu", 0.0, arx1.symuluj(0.0));


    RegulatorPID pid1(100.0, 0.0, 0.0, 1.0, -5.0, 5.0);
    check("4. PID: Saturacja sterowania", 5.0, pid1.symuluj(1.0));


    pid1.resetCalki();
    check("5. PID: Reset calki", 0.0, pid1.symuluj(0.0));


    SygnalProstokatny gen(1.0, 10.0, 0.5, 0.0);
    check("6. Generator: Poziom niski", 0.0, gen.generuj(7.5));


    MenadzerSymulacji m(1.0, 0, 0, 1.0, -100, 100, {-0.5}, {0.5}, 1, 0);

    m.setTypGeneratora(MenadzerSymulacji::TypGeneratora::Prostokatny);
    m.setParametryGeneratoraProstokatnego(1.0, 100, 0.5, 0.0);

    m.startSymulacji();
    m.wykonajKrokSymulacji(0.0);
    m.wykonajKrokSymulacji(1.0);

    total_tests++;
    if (m.getWartoscRegulowana() > 0) {
        passed_tests++;
    } else {
        std::cerr << "[FAIL] 7. Menadzer: Brak reakcji obiektu w petli" << std::endl;
    }


    m.stopSymulacji();
    total_tests++;
    if (!m.czySymulacjaUruchomiona()) {
        passed_tests++;
    } else {
        std::cerr << "[FAIL] 8. Menadzer: Blad zatrzymania" << std::endl;
    }


    m.setOgraniczeniaARX(true);
    m.setOgraniczeniaRegulowanej(-0.1, 0.1);
    m.startSymulacji();

    for(int i=0; i<5; i++) m.wykonajKrokSymulacji(i);
    check("9. Menadzer: Saturacja petli", 0.1, m.getWartoscRegulowana());


    m.resetSymulacji();
    check("10. Menadzer: Reset uchybu", 0.0, m.getUchyb());
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(QCommandLineOption("test", "Uruchom testy i zakoncz"));
    parser.addOption(QCommandLineOption("serwer", "Uruchom testowy serwer sieciowy")); // Zamiast --sieci
    parser.addOption(QCommandLineOption("klient", "Uruchom testowy klient sieciowy"));  // Zamiast --sieci
    parser.process(a);

    if (parser.isSet("test")) {
        std::cout << "start testów" << std::endl;

        wykonaj_moje_testy();

        std::cout << "\n";

        wykonaj_testy_prowadzacego();

        std::cout << "WYNIK KONCOWY: " << passed_tests << " / " << total_tests
                  << " testow zaliczonych." << std::endl;

        return 0;
    }
    if (parser.isSet("serwer")) {
        qDebug() << "--- TRYB KONSOLOWY: SERWER ---";
        MyTCPServer* server = new MyTCPServer(&a);

        if (server->startListening(12345)) {
            qDebug() << "[SERWER] Czekam na porcie 12345...";
        }

        QObject::connect(server, &MyTCPServer::newClientConnected, [](QString adr){
            qDebug() << "[SERWER] Podlaczyl sie klient:" << adr;
        });

        QObject::connect(server, &MyTCPServer::nowaRamkaOd, [server](int typ, QByteArray payload, int numCli){
            qDebug() << "[SERWER] Otrzymano ramke typu:" << typ << "od klienta nr:" << numCli;

            if (typ == 1) {
                RegulatorPID odebranyPid(0.0);
                quint32 czas = deserializePID(payload, odebranyPid);
                qDebug() << "[SERWER] Rozkodowano PID. Kp =" << odebranyPid.getKp() << "| Czas:" << czas;

                // --- NOWE: Odsyłanie ARX do klienta ---
                qDebug() << "[SERWER] Odsylam obiekt ARX jako odpowiedz...";
                ModelARX testowyArx({-0.5}, {2.5}, 1, 0);
                QByteArray payloadArx = serializeARX(testowyArx, 999); // 999 to testowy timestamp

                server->wyslijRamke(2, payloadArx, numCli); // Typ 2 = ARX
            }
        });

        return a.exec();
    }

    if (parser.isSet("klient")) {
        qDebug() << "--- TRYB KONSOLOWY: KLIENT ---";
        MyTCPClient* client = new MyTCPClient(&a);

        qDebug() << "[KLIENT] Laczenie z 127.0.0.1:12345...";
        client->connectTo("127.0.0.1", 12345);

        QObject::connect(client, &MyTCPClient::connected, [client](){
            qDebug() << "[KLIENT] Polaczono. Wysylam obiekt PID.";
            RegulatorPID testowyPid(12.34, 5.0, 0.1, 1.0, -100, 100);

            quint32 aktualnyCzas = 555; // Np. z managera symulacji
            QByteArray payload = serializePID(testowyPid, aktualnyCzas);

            client->wyslijRamke(1, payload); // 1 = Typ wiadomości (PID)
        });

        return a.exec();
    }
    MainWindow w;
    w.show();
    return a.exec();
}

#pragma once
#include <QMainWindow>
#include <QElapsedTimer>
#include <QLabel>
#include "MenadzerSymulacji.h"
#include "dialogarx.h"
#include "ZarzadzanieCzasem.h"
#include "W_SIECIOWA/MenadzerSieci.h"
#include "W_SIECIOWA/DialogPolaczenia.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ---- symulacja ----
    void onNoweData(double t, double y_zad, double y, double u, double e,
                    RegulatorPID::Skladowe skladowe);
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnReset_clicked();

    // ---- PID ----
    void on_btnUpdatePID_clicked();
    void on_btnResetIntegral_clicked();
    void on_comboPidType_currentIndexChanged(int index);
    void updatePidParams();

    // ---- ARX ----
    void on_btnOpenARX_clicked();

    // ---- Generator ----
    void updateGeneratorParams();

    // ---- Czas / okno ----
    void onIntervalChanged();
    void onWindowChanged();

    // ---- Zapis / Odczyt ----
    void on_btnSave_clicked();
    void on_btnLoad_clicked();

    // ---- Sieć ----
    void on_btnPolacz_clicked();       // otwiera DialogPolaczenia
    void on_btnRozlacz_clicked();      // rozłącza z potwierdzeniem
    void onPolaczono(QString ip, int port, bool jakoSerwer);
    void onRozlaczenieZewnetrzne();
    void onKonfiguracjaOdebrana(TypRamki typ);

    // ---- Nasłuch zmian konfiguracji do wysyłki ----
    void onLocalPidChanged();
    void onLocalArxChanged();
    void onLocalGeneratorChanged();

private:
    Ui::MainWindow *ui;
    MenadzerSymulacji *manager;
    MenadzerSieci     *menadzerSieci;
    DialogARX         *arxDialog;
    DialogPolaczenia  *dialogPolaczenia;

    // Etykieta statusu sieci w statusbarze
    QLabel* _labelStatusSieci;

    void setupPlots();
    void updatePlots(double t, double y_zad, double y, double u, double e,
                     RegulatorPID::Skladowe pidParts);

    // Blokowanie/odblokowywanie kontrolek zależnie od trybu i roli
    void zastosujBlokadyTrybuSieciowego(RolaSieciowa rola);
    void odblokujWszystko();

    // Aktualizacja GUI po odebraniu konfiguracji z sieci
    void odswiezGUIPID();
    void odswiezGUIGenerator();

    bool _trybSieciowy = false;
};

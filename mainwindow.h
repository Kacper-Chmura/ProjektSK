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
    void onNoweData(double t, double y_zad, double y, double u, double e,
                    RegulatorPID::Skladowe skladowe);
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnReset_clicked();

    void on_btnUpdatePID_clicked();
    void on_btnResetIntegral_clicked();
    void on_comboPidType_currentIndexChanged(int index);
    void updatePidParams();

    void on_btnOpenARX_clicked();

    void updateGeneratorParams();

    void onIntervalChanged();
    void onWindowChanged();

    void on_btnSave_clicked();
    void on_btnLoad_clicked();

    void on_btnPolacz_clicked();
    void on_btnRozlacz_clicked();
    void onPolaczono(QString ip, int port, bool jakoSerwer);
    void onRozlaczenieZewnetrzne();
    void onKonfiguracjaOdebrana(TypRamki typ);

    void onLocalPidChanged();
    void onLocalArxChanged();
    void onLocalGeneratorChanged();
    void onSygnalWydajnosci(bool wyrabiaSie);

private:
    Ui::MainWindow *ui;
    MenadzerSymulacji *manager;
    MenadzerSieci     *menadzerSieci;
    DialogARX         *arxDialog;
    DialogPolaczenia  *dialogPolaczenia;

    QLabel* _labelStatusSieci;

    void setupPlots();
    void updatePlots(double t, double y_zad, double y, double u, double e,
                     RegulatorPID::Skladowe pidParts);

    void zastosujBlokadyTrybuSieciowego(RolaSieciowa rola);
    void odblokujWszystko();

    void odswiezGUIPID();
    void odswiezGUIGenerator();
    void odswiezGUIARX();

    bool _trybSieciowy = false;
    QLabel* _labelWydajnosc;
};

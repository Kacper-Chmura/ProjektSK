#pragma once
#include <QMainWindow>
#include <QElapsedTimer>
#include "MenadzerSymulacji.h"
#include "dialogarx.h"
#include "ZarzadzanieCzasem.h"
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
    void on_btnOpenARX_clicked();
    void on_btnSave_clicked();
    void on_btnLoad_clicked();
    void on_btnResetIntegral_clicked();
    void on_comboPidType_currentIndexChanged(int index);

    void updatePidParams();
    void onIntervalChanged();

    void updateGeneratorParams();

    void onWindowChanged();

private:
    Ui::MainWindow *ui;
    MenadzerSymulacji *manager;
    DialogARX *arxDialog;

    void setupPlots();
    void updatePlots(double t, double y_zad, double y, double u, double e, RegulatorPID::Skladowe pidParts);
};

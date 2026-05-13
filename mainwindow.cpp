#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "json.hpp"
#include <fstream>
#include <QMessageBox>
#include <QFileDialog>
#include <QNetworkInterface>
#include "./ZarzadzanieCzasem.h"

using json = nlohmann::json;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::vector<double> startA = {-0.4};
    std::vector<double> startB = {0.6};

    manager = new MenadzerSymulacji(0.5, 10.0, 0.0, 0.05, -10.0, 10.0,
                                    startA, startB, 1, 0.0, this);

    connect(manager, &MenadzerSymulacji::noweDataReady,
            this, &MainWindow::onNoweData, Qt::DirectConnection);

    menadzerSieci = new MenadzerSieci(manager, this);
    connect(menadzerSieci, &MenadzerSieci::polaczonySygnal,      this, &MainWindow::onPolaczono);
    connect(menadzerSieci, &MenadzerSieci::rozlaczenieZewnetrzne, this, &MainWindow::onRozlaczenieZewnetrzne);
    connect(menadzerSieci, &MenadzerSieci::konfiguracjaOdebrana,  this, &MainWindow::onKonfiguracjaOdebrana);

    arxDialog        = new DialogARX(this);
    dialogPolaczenia = new DialogPolaczenia(this);

    arxDialog->setParams(startA, startB, 1, 0.0);
    arxDialog->setOgraniczenia(true, -10.0, 10.0, -10.0, 10.0);

    setupPlots();

    ui->spinKp->setValue(0.5);
    ui->spinTi->setValue(10.0);
    ui->spinTd->setValue(0.0);
    ui->spinTp->setMinimum(10);
    ui->spinTp->setMaximum(500);
    ui->spinTp->setValue(50);
    ui->spinTp->setSuffix(" ms");

    connect(ui->spinTp, &QDoubleSpinBox::editingFinished, this, &MainWindow::onIntervalChanged);
    connect(ui->spinKp, &QDoubleSpinBox::editingFinished, this, &MainWindow::updatePidParams);
    connect(ui->spinTi, &QDoubleSpinBox::editingFinished, this, &MainWindow::updatePidParams);
    connect(ui->spinTd, &QDoubleSpinBox::editingFinished, this, &MainWindow::updatePidParams);

    ui->spinFill->setValue(0.5);
    connect(ui->spinAmp,    &QSpinBox::editingFinished,         this, &MainWindow::updateGeneratorParams);
    connect(ui->spinPeriod, &QSpinBox::editingFinished,         this, &MainWindow::updateGeneratorParams);
    connect(ui->spinFill,   &QDoubleSpinBox::editingFinished,   this, &MainWindow::updateGeneratorParams);
    connect(ui->spinOffset, &QDoubleSpinBox::editingFinished,   this, &MainWindow::updateGeneratorParams);
    connect(ui->comboGenType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updateGeneratorParams);

    ui->spinWindow->setValue(10);
    connect(ui->spinWindow, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onWindowChanged);

    _labelStatusSieci = new QLabel("  Tryb: stacjonarny  ", this);
    _labelStatusSieci->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->addPermanentWidget(_labelStatusSieci);

    _labelWydajnosc = new QLabel(this);
    _labelWydajnosc->setFixedSize(20, 20);
    _labelWydajnosc->setStyleSheet("background-color: gray; border: 1px solid black;");
    statusBar()->addPermanentWidget(_labelWydajnosc);

    connect(manager, &MenadzerSymulacji::sygnalWydajnosci, this, &MainWindow::onSygnalWydajnosci);
}

MainWindow::~MainWindow()
{
    delete manager;
    delete ui;
}

void MainWindow::setupPlots()
{
    ui->plotMain->addGraph();
    ui->plotMain->graph(0)->setPen(QPen(Qt::red));
    ui->plotMain->graph(0)->setName("Wartość Zadana (w)");

    ui->plotMain->addGraph();
    ui->plotMain->graph(1)->setPen(QPen(Qt::blue));
    ui->plotMain->graph(1)->setName("Wartość Regulowana (y)");

    ui->plotMain->yAxis->setLabel("w, y");
    ui->plotMain->legend->setVisible(true);
    ui->plotMain->legend->setBrush(Qt::NoBrush);
    ui->plotMain->legend->setBorderPen(Qt::NoPen);

    ui->plotError->addGraph();
    ui->plotError->graph(0)->setPen(QPen(Qt::black));
    ui->plotError->graph(0)->setName("Uchyb (e)");
    ui->plotError->yAxis->setLabel("Uchyb e");

    ui->plotControl->addGraph();
    ui->plotControl->graph(0)->setPen(QPen(Qt::darkGreen));
    ui->plotControl->graph(0)->setName("Sterowanie (u)");
    ui->plotControl->yAxis->setLabel("Sterowanie u");

    ui->plotPID->addGraph();
    ui->plotPID->graph(0)->setPen(QPen(Qt::green));
    ui->plotPID->graph(0)->setName("P");

    ui->plotPID->addGraph();
    ui->plotPID->graph(1)->setPen(QPen(Qt::darkYellow));
    ui->plotPID->graph(1)->setName("I");

    ui->plotPID->addGraph();
    ui->plotPID->graph(2)->setPen(QPen(Qt::magenta));
    ui->plotPID->graph(2)->setName("D");

    ui->plotPID->yAxis->setLabel("Składowe PID");
    ui->plotPID->legend->setVisible(true);
    ui->plotPID->legend->setBrush(Qt::NoBrush);
    ui->plotPID->legend->setBorderPen(Qt::NoPen);

    QVector<QCustomPlot*> plots = {ui->plotMain, ui->plotError, ui->plotControl, ui->plotPID};
    for (auto plot : plots) {
        plot->xAxis->setLabel("Czas [s]");
        plot->setInteraction(QCP::iRangeDrag, true);
        plot->setInteraction(QCP::iRangeZoom, true);
    }
}

void MainWindow::updatePlots(double t, double y_zad, double y, double u, double e,
                             RegulatorPID::Skladowe pid)
{
    ui->plotMain->graph(0)->addData(t, y_zad);
    ui->plotMain->graph(1)->addData(t, y);
    ui->plotError->graph(0)->addData(t, e);
    ui->plotControl->graph(0)->addData(t, u);
    ui->plotPID->graph(0)->addData(t, pid.P);
    ui->plotPID->graph(1)->addData(t, pid.I);
    ui->plotPID->graph(2)->addData(t, pid.D);

    // Zabezpieczenie przed window = 0
    double window = qMax(1.0, (double)ui->spinWindow->value());

    QVector<QCustomPlot*> plots = {ui->plotMain, ui->plotError, ui->plotControl, ui->plotPID};
    for (auto plot : plots) {
        // Oś X przesuwa się w prawo, zapewniając miejsce po prawej aż czas t przekroczy wielkość okna
        plot->xAxis->setRange(qMax(0.0, t - window), qMax(window, t));

        // Trzymaj szeroki bufor danych (np. 1000 sekund) zamiast usuwać je dynamicznie wraz z oknem
        // Dzięki temu po powiększeniu okna w locie, wykres nie będzie miał "pustej dziury"
        for (int i = 0; i < plot->graphCount(); ++i)
            plot->graph(i)->data()->removeBefore(t - 1000.0);

        plot->yAxis->rescale();
        QCPRange range = plot->yAxis->range();
        double center = range.center();
        double size   = (range.size() == 0.0) ? 1.0 : range.size();
        plot->yAxis->setRange(center - size * 0.6, center + size * 0.6);

        plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void MainWindow::onNoweData(double t, double y_zad, double y, double u, double e,
                             RegulatorPID::Skladowe skladowe)
{
    updatePlots(t, y_zad, y, u, e, skladowe);
}

void MainWindow::on_btnStart_clicked()
{
    int intervalMs = ui->spinTp->value();
    manager->getZarzadzanieCzasem()->setInterwalMs(intervalMs);
    updateGeneratorParams();
    on_btnUpdatePID_clicked();
    manager->startSymulacji();
}

void MainWindow::on_btnStop_clicked()
{
    manager->stopSymulacji();
}

void MainWindow::on_btnReset_clicked()
{
    on_btnStop_clicked();
    manager->resetSymulacji();
    manager->resetPamieciPID();

    QVector<QCustomPlot*> plots = {ui->plotMain, ui->plotError, ui->plotControl, ui->plotPID};
    for (auto plot : plots) {
        for (int i = 0; i < plot->graphCount(); ++i)
            plot->graph(i)->data()->clear();
        plot->replot();
    }

    if (_trybSieciowy && menadzerSieci->getRole() == RolaSieciowa::Regulator) {
        menadzerSieci->wyslijReset();
    }
}
void MainWindow::on_btnUpdatePID_clicked()
{
    double Kp = ui->spinKp->value();
    double Ti = ui->spinTi->value();
    double Td = ui->spinTd->value();
    double Tp = ui->spinTp->value() / 1000.0;
    manager->setNastawyPID(Kp, Ti, Td, -10.0, 10.0, Tp);

    if (_trybSieciowy && menadzerSieci->getRole() == RolaSieciowa::Regulator)
        menadzerSieci->wyslijKonfiguracjePID();
}

void MainWindow::updatePidParams()
{
    on_btnUpdatePID_clicked();
}

void MainWindow::on_btnResetIntegral_clicked()
{
    manager->resetCalkiPID();
}

void MainWindow::on_comboPidType_currentIndexChanged(int index)
{
    if (index == 0) manager->setLiczCalkPID(RegulatorPID::LiczCalk::Zew);
    else            manager->setLiczCalkPID(RegulatorPID::LiczCalk::Wew);
}

void MainWindow::on_btnOpenARX_clicked()
{
    odswiezGUIARX();

    if (arxDialog->exec() == QDialog::Accepted) {
        manager->setParametryARX(arxDialog->getA(), arxDialog->getB(),
                                 arxDialog->getK(), arxDialog->getPozSz());
        manager->setOgraniczeniaARX(arxDialog->getOgraniczeniaWlaczone());
        manager->setOgraniczeniaSterowania(arxDialog->getUMin(), arxDialog->getUMax());
        manager->setOgraniczeniaRegulowanej(arxDialog->getYMin(), arxDialog->getYMax());

        if (_trybSieciowy)
            menadzerSieci->wyslijKonfiguracjeARX();
    }
}

void MainWindow::updateGeneratorParams()
{
    double amp    = ui->spinAmp->value();
    double per    = ui->spinPeriod->value();
    double fill   = ui->spinFill->value();
    double offset = ui->spinOffset->value();

    if (ui->comboGenType->currentIndex() == 1) {
        manager->setTypGeneratora(MenadzerSymulacji::TypGeneratora::Sinusoidalny);
        manager->setParametryGeneratoraSinusoidalnego(amp, per, offset);
    } else {
        manager->setTypGeneratora(MenadzerSymulacji::TypGeneratora::Prostokatny);
        manager->setParametryGeneratoraProstokatnego(amp, per, fill, offset);
    }

    if (_trybSieciowy && menadzerSieci->getRole() == RolaSieciowa::Regulator)
        menadzerSieci->wyslijKonfiguracjeGeneratora();
}

void MainWindow::onIntervalChanged()
{
    int    intervalMs = ui->spinTp->value();
    double Tp         = intervalMs / 1000.0;
    manager->getZarzadzanieCzasem()->setInterwalMs(intervalMs);
    manager->setNastawyPID(ui->spinKp->value(), ui->spinTi->value(),
                           ui->spinTd->value(), -10.0, 10.0, Tp);

    if (_trybSieciowy && menadzerSieci->getRole() == RolaSieciowa::Regulator)
        menadzerSieci->wyslijKonfiguracjePID();
}

void MainWindow::onWindowChanged()
{
}

void MainWindow::on_btnSave_clicked()
{
    json j;
    j["PID"] = {
        {"Kp", ui->spinKp->value()}, {"Ti", ui->spinTi->value()},
        {"Td", ui->spinTd->value()}, {"Tp", ui->spinTp->value()}
    };
    j["ARX"] = {
        {"k", arxDialog->getK()}, {"noise", arxDialog->getPozSz()},
        {"A", arxDialog->getA()}, {"B", arxDialog->getB()},
        {"ograniczenia_wlaczone", arxDialog->getOgraniczeniaWlaczone()},
        {"u_min", arxDialog->getUMin()}, {"u_max", arxDialog->getUMax()},
        {"y_min", arxDialog->getYMin()}, {"y_max", arxDialog->getYMax()}
    };
    j["Gen"] = {
        {"Type", ui->comboGenType->currentIndex()},
        {"Amp",  ui->spinAmp->value()}, {"Per", ui->spinPeriod->value()},
        {"Fill", ui->spinFill->value()}, {"Offset", ui->spinOffset->value()}
    };

    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz konfigurację", "", "JSON (*.json)");
    if (fileName.isEmpty()) return;
    std::ofstream o(fileName.toStdString());
    o << j.dump(4);
}

void MainWindow::on_btnLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Wczytaj konfigurację", "", "JSON (*.json)");
    if (fileName.isEmpty()) return;

    std::ifstream i(fileName.toStdString());
    json j;
    if (!(i >> j)) return;

    ui->spinKp->setValue(j["PID"]["Kp"]);
    ui->spinTi->setValue(j["PID"]["Ti"]);
    ui->spinTd->setValue(j["PID"]["Td"]);
    ui->spinTp->setValue(j["PID"]["Tp"]);

    ui->comboGenType->setCurrentIndex(j["Gen"]["Type"]);
    ui->spinAmp->setValue(j["Gen"]["Amp"].get<int>());
    ui->spinPeriod->setValue(j["Gen"]["Per"].get<int>());
    ui->spinFill->setValue(j["Gen"]["Fill"]);
    ui->spinOffset->setValue(j["Gen"]["Offset"]);

    arxDialog->setParams(
        j["ARX"]["A"].get<std::vector<double>>(),
        j["ARX"]["B"].get<std::vector<double>>(),
        j["ARX"]["k"],
        j["ARX"]["noise"]
        );

    if (j["ARX"].contains("ograniczenia_wlaczone")) {
        arxDialog->setOgraniczenia(
            j["ARX"]["ograniczenia_wlaczone"],
            j["ARX"]["u_min"], j["ARX"]["u_max"],
            j["ARX"]["y_min"], j["ARX"]["y_max"]
            );
    }

    manager->setParametryARX(arxDialog->getA(), arxDialog->getB(),
                              j["ARX"]["k"], j["ARX"]["noise"]);
    if (j["ARX"].contains("ograniczenia_wlaczone")) {
        manager->setOgraniczeniaARX(j["ARX"]["ograniczenia_wlaczone"]);
        manager->setOgraniczeniaSterowania(j["ARX"]["u_min"], j["ARX"]["u_max"]);
        manager->setOgraniczeniaRegulowanej(j["ARX"]["y_min"], j["ARX"]["y_max"]);
    }
}

void MainWindow::on_btnPolacz_clicked()
{
    ui->btnPolacz->setEnabled(false);
    ui->btnPolacz->setEnabled(true);

    if (dialogPolaczenia->exec() != QDialog::Accepted)
        return;

    KonfiguracjaPolaczenia kp = dialogPolaczenia->getKonfiguracja();

    RolaSieciowa rola = (kp.rola == 0) ? RolaSieciowa::Regulator : RolaSieciowa::Obiekt;
    menadzerSieci->setRole(rola);

    if (kp.czyJestSerwer) {
        if (!menadzerSieci->startujSerwer(kp.port)) {
            QMessageBox::critical(this, "Błąd", "Nie można uruchomić serwera na porcie "
                                                + QString::number(kp.port) + ".");
            return;
        }
        _labelStatusSieci->setText(QString("  Serwer – nasłuchuję na porcie %1...  ").arg(kp.port));
        ui->btnPolacz->setEnabled(false);
        ui->btnRozlacz->setEnabled(true);
    } else {
        menadzerSieci->polaczJakoKlient(kp.adresIP, kp.port);
        _labelStatusSieci->setText(QString("  Klient – łączę z %1:%2...  ")
                                       .arg(kp.adresIP).arg(kp.port));
        ui->btnPolacz->setEnabled(false);
        ui->btnRozlacz->setEnabled(true);
    }
}

void MainWindow::on_btnRozlacz_clicked()
{
    int ret = QMessageBox::question(
        this, "Rozłączenie",
        "Czy na pewno chcesz przełączyć w tryb stacjonarny?\n"
        "Jeśli symulacja jest uruchomiona, będzie kontynuowana lokalnie.",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes) return;

    menadzerSieci->rozlacz();

    _trybSieciowy = false;
    odblokujWszystko();

    ui->btnPolacz->setEnabled(true);
    ui->btnRozlacz->setEnabled(false);
    _labelStatusSieci->setText("  Tryb: stacjonarny  ");
}

void MainWindow::onPolaczono(QString ip, int port, bool jakoSerwer)
{
    _trybSieciowy = true;

    RolaSieciowa rola = menadzerSieci->getRole();

    MenadzerSymulacji::TrybPracy tryb = (rola == RolaSieciowa::Regulator)
                                            ? MenadzerSymulacji::TrybPracy::SiecRegulator
                                            : MenadzerSymulacji::TrybPracy::SiecObiekt;
    manager->setTrybPracy(tryb);

    if (rola == RolaSieciowa::Regulator) {
        updatePidParams();
        updateGeneratorParams();
    }

    zastosujBlokadyTrybuSieciowego(rola);

    menadzerSieci->wyslijPelnaKonfiguracje();

    QString rolaNazwa = (rola == RolaSieciowa::Regulator) ? "Regulator" : "Obiekt";
    QString trybNazwa = jakoSerwer ? "Serwer" : "Klient";

    _labelStatusSieci->setText(
        QString("  [%1/%2] Połączono z: %3 (port %4)  ")
            .arg(rolaNazwa).arg(trybNazwa).arg(ip).arg(port));

    QMessageBox::information(
        this, "Połączono",
        QString("Połączono z instancją pod adresem: %1\nRola: %2")
            .arg(ip).arg(rolaNazwa));
}

void MainWindow::onRozlaczenieZewnetrzne()
{
    manager->setTrybPracy(MenadzerSymulacji::TrybPracy::Stacjonarny);
    _trybSieciowy = false;
    odblokujWszystko();

    ui->btnPolacz->setEnabled(true);
    ui->btnRozlacz->setEnabled(false);
    _labelStatusSieci->setText("  Tryb: stacjonarny (utracono połączenie)  ");

    QMessageBox::warning(this, "Utracono połączenie",
                         "Połączenie z drugą instancją zostało zerwane!\n"
                         "Aplikacja przeszła w tryb stacjonarny.\n"
                         "Jeśli symulacja była uruchomiona, działa dalej na ustawieniach lokalnych.");
}

void MainWindow::onKonfiguracjaOdebrana(TypRamki typ)
{
    switch (typ) {
    case TypRamki::PID:
        odswiezGUIPID();
        break;
    case TypRamki::Generator:
        odswiezGUIGenerator();
        break;
    case TypRamki::ARX:
        odswiezGUIARX();
        break;
    case TypRamki::InfoPolaczenia:
        if (_trybSieciowy) {
            QString ip   = menadzerSieci->getZdalneIP();
            int    port  = menadzerSieci->getZdalnyPort();
            bool   srv   = menadzerSieci->czyJestSerwerem();
            QString rola = (menadzerSieci->getRole() == RolaSieciowa::Regulator) ? "Regulator" : "Obiekt";
            QString tryb = srv ? "Serwer" : "Klient";
            _labelStatusSieci->setText(
                QString("  [%1/%2] Połączono z: %3 (port %4)  ")
                    .arg(rola).arg(tryb).arg(ip).arg(port));
        }
        break;
    case TypRamki::Reset:
        manager->resetSymulacji();
        manager->resetPamieciPID();

        for (auto plot : {ui->plotMain, ui->plotError, ui->plotControl, ui->plotPID}) {
            for (int i = 0; i < plot->graphCount(); ++i)
                plot->graph(i)->data()->clear();
            plot->replot();
        }
        break;
    default:
        break;
    }
}

void MainWindow::odswiezGUIARX()
{
    ModelARX* arx = manager->getARX();
    if (arx) {
        arxDialog->setParams(arx->getA(), arx->getB(), arx->getk(), arx->getpozsz());
        arxDialog->setOgraniczenia(
            arx->ograniczenia(),
            arx->getUMin(), arx->getUMax(),
            arx->getYMin(), arx->getYMax()
            );
    }
}

void MainWindow::zastosujBlokadyTrybuSieciowego(RolaSieciowa rola)
{
    if (rola == RolaSieciowa::Obiekt) {
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(false);
        ui->btnReset->setEnabled(false);
        ui->spinKp->setEnabled(false);
        ui->spinTi->setEnabled(false);
        ui->spinTd->setEnabled(false);
        ui->spinTp->setEnabled(false);
        ui->btnUpdatePID->setEnabled(false);
        ui->btnResetIntegral->setEnabled(false);
        ui->comboPidType->setEnabled(false);
        ui->spinAmp->setEnabled(false);
        ui->spinPeriod->setEnabled(false);
        ui->spinFill->setEnabled(false);
        ui->spinOffset->setEnabled(false);
        ui->comboGenType->setEnabled(false);
        ui->btnSave->setEnabled(false);
        ui->btnLoad->setEnabled(false);
        ui->btnOpenARX->setEnabled(true);
        ui->btnRozlacz->setEnabled(true);
    } else {
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(true);
        ui->btnReset->setEnabled(true);
        ui->spinKp->setEnabled(true);
        ui->spinTi->setEnabled(true);
        ui->spinTd->setEnabled(true);
        ui->spinTp->setEnabled(true);
        ui->btnUpdatePID->setEnabled(true);
        ui->btnResetIntegral->setEnabled(true);
        ui->comboPidType->setEnabled(true);
        ui->spinAmp->setEnabled(true);
        ui->spinPeriod->setEnabled(true);
        ui->spinFill->setEnabled(true);
        ui->spinOffset->setEnabled(true);
        ui->comboGenType->setEnabled(true);
        ui->btnSave->setEnabled(true);
        ui->btnLoad->setEnabled(true);
        ui->btnOpenARX->setEnabled(false);
        ui->btnRozlacz->setEnabled(true);
    }
}

void MainWindow::odblokujWszystko()
{
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(true);
    ui->btnReset->setEnabled(true);
    ui->spinKp->setEnabled(true);
    ui->spinTi->setEnabled(true);
    ui->spinTd->setEnabled(true);
    ui->spinTp->setEnabled(true);
    ui->btnUpdatePID->setEnabled(true);
    ui->btnResetIntegral->setEnabled(true);
    ui->comboPidType->setEnabled(true);
    ui->spinAmp->setEnabled(true);
    ui->spinPeriod->setEnabled(true);
    ui->spinFill->setEnabled(true);
    ui->spinOffset->setEnabled(true);
    ui->comboGenType->setEnabled(true);
    ui->btnSave->setEnabled(true);
    ui->btnLoad->setEnabled(true);
    ui->btnOpenARX->setEnabled(true);

    _labelWydajnosc->setStyleSheet("background-color: gray; border: 1px solid black;");
}

void MainWindow::odswiezGUIPID()
{
    const RegulatorPID* pid = manager->getPID();
    ui->spinKp->blockSignals(true);
    ui->spinTi->blockSignals(true);
    ui->spinTd->blockSignals(true);
    ui->spinTp->blockSignals(true);

    ui->spinKp->setValue(pid->getKp());
    ui->spinTi->setValue(pid->getTi());
    ui->spinTd->setValue(pid->getTd());
    ui->spinTp->setValue(pid->getTp() * 1000.0);

    ui->spinKp->blockSignals(false);
    ui->spinTi->blockSignals(false);
    ui->spinTd->blockSignals(false);
    ui->spinTp->blockSignals(false);
}

void MainWindow::odswiezGUIGenerator()
{
    int typGenGUI = (manager->getTypGeneratora() == MenadzerSymulacji::TypGeneratora::Sinusoidalny) ? 1 : 0;

    ui->comboGenType->blockSignals(true);
    ui->spinAmp->blockSignals(true);
    ui->spinPeriod->blockSignals(true);
    ui->spinFill->blockSignals(true);
    ui->spinOffset->blockSignals(true);

    ui->comboGenType->setCurrentIndex(typGenGUI);
    ui->spinAmp->setValue(static_cast<int>(manager->getGenAmplituda()));
    ui->spinPeriod->setValue(static_cast<int>(manager->getGenOkres()));
    ui->spinFill->setValue(manager->getGenWypelnienie());
    ui->spinOffset->setValue(manager->getGenSkladowaStala());

    ui->comboGenType->blockSignals(false);
    ui->spinAmp->blockSignals(false);
    ui->spinPeriod->blockSignals(false);
    ui->spinFill->blockSignals(false);
    ui->spinOffset->blockSignals(false);
}

void MainWindow::onLocalPidChanged()     { if (_trybSieciowy) menadzerSieci->wyslijKonfiguracjePID(); }
void MainWindow::onLocalArxChanged()     { if (_trybSieciowy) menadzerSieci->wyslijKonfiguracjeARX(); }
void MainWindow::onLocalGeneratorChanged(){ if (_trybSieciowy) menadzerSieci->wyslijKonfiguracjeGeneratora(); }
void MainWindow::onSygnalWydajnosci(bool wyrabiaSie)
{
    if (wyrabiaSie) {
        _labelWydajnosc->setStyleSheet("background-color: green; border: 1px solid black;");
    } else {
        _labelWydajnosc->setStyleSheet("background-color: red; border: 1px solid black;");
    }
}

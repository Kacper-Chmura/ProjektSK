#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "json.hpp"
#include <fstream>
#include <QMessageBox>
#include <QFileDialog>

using json = nlohmann::json;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::vector<double> startA = {-0.4};
    std::vector<double> startB = {0.6};

    manager = new MenadzerSymulacji(0.5, 10.0, 0.1, 0.2, -10.0, 10.0, startA, startB, 1, 0.0, this);

    connect(manager, &MenadzerSymulacji::noweDataReady,
            this, &MainWindow:: onNoweData, Qt::DirectConnection);

    arxDialog = new DialogARX(this);

    arxDialog->setParams(startA, startB, 1, 0.0);
    arxDialog->setOgraniczenia(true, -10.0, 10.0, -10.0, 10.0);

    setupPlots();

    ui->spinKp->setValue(0.5);
    ui->spinTi->setValue(10.0);
    ui->spinTd->setValue(0.1);
    ui->spinTp->setMinimum(10);
    ui->spinTp->setMaximum(1000);
    ui->spinTp->setValue(200);
    ui->spinTp->setSuffix(" ms");

    connect(ui->spinTp, &QDoubleSpinBox::editingFinished,
            this, &MainWindow::onIntervalChanged);

    ui->spinFill->setValue(0.5);

    connect(ui->spinKp, &QDoubleSpinBox::editingFinished,
            this, &MainWindow::updatePidParams);
    connect(ui->spinTi, &QDoubleSpinBox::editingFinished,
            this, &MainWindow::updatePidParams);
    connect(ui->spinTd, &QDoubleSpinBox::editingFinished,
            this, &MainWindow::updatePidParams);

    connect(ui->spinAmp, &QSpinBox::editingFinished, this, &MainWindow::updateGeneratorParams);
    connect(ui->spinPeriod, &QSpinBox::editingFinished, this, &MainWindow::updateGeneratorParams);
    connect(ui->spinFill, &QDoubleSpinBox::editingFinished, this, &MainWindow:: updateGeneratorParams);
    connect(ui->spinOffset, &QDoubleSpinBox::editingFinished, this, &MainWindow:: updateGeneratorParams);
    connect(ui->comboGenType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow:: updateGeneratorParams);

    ui->spinWindow->setValue(10);

    connect(ui->spinWindow, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onWindowChanged);
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
    ui->plotMain->graph(0)->setName("Wartość Zadana");

    QCPItemText *labelMain = new QCPItemText(ui->plotMain);
    labelMain->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    labelMain->position->setType(QCPItemPosition::ptAxisRectRatio);
    labelMain->position->setCoords(0.5, -0.03);
    labelMain->setClipToAxisRect(false);
    labelMain->setFont(QFont("Arial", 11));
    labelMain->setColor(Qt::black);
    labelMain->setText("Wykres Główny");

    ui->plotMain->addGraph();
    ui->plotMain->graph(1)->setPen(QPen(Qt::blue));
    ui->plotMain->graph(1)->setName("Wyjście Y");

    ui->plotMain->yAxis->setLabel("Y / Zadana");
    ui->plotMain->legend->setVisible(true);

    ui->plotMain->legend->setBrush(Qt::NoBrush);
    ui->plotMain->legend->setBorderPen(Qt::NoPen);

    ui->plotError->addGraph();
    ui->plotError->graph(0)->setPen(QPen(Qt::black));
    ui->plotError->graph(0)->setName("Uchyb");
    ui->plotError->yAxis->setLabel("Uchyb");

    QCPItemText *labelUchyb = new QCPItemText(ui->plotError);
    labelUchyb->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    labelUchyb->position->setType(QCPItemPosition::ptAxisRectRatio);
    labelUchyb->position->setCoords(0.5, -0.04);
    labelUchyb->setClipToAxisRect(false);
    labelUchyb->setFont(QFont("Arial", 10));
    labelUchyb->setColor(Qt::black);
    labelUchyb->setText("Wykres Uchybu");

    ui->plotControl->addGraph();
    ui->plotControl->graph(0)->setPen(QPen(Qt::darkGreen));
    ui->plotControl->graph(0)->setName("Sterowanie U");
    ui->plotControl->yAxis->setLabel("Sterowanie");

    QCPItemText *labelSterowanie = new QCPItemText(ui->plotControl);
    labelSterowanie->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    labelSterowanie->position->setType(QCPItemPosition::ptAxisRectRatio);
    labelSterowanie->position->setCoords(0.5, -0.04);
    labelSterowanie->setClipToAxisRect(false);
    labelSterowanie->setFont(QFont("Arial", 10));
    labelSterowanie->setColor(Qt::black);
    labelSterowanie->setText("Wykres Sterowania");

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

    QCPItemText *labelPID = new QCPItemText(ui->plotPID);
    labelPID->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    labelPID->position->setType(QCPItemPosition::ptAxisRectRatio);
    labelPID->position->setCoords(0.5, -0.04);
    labelPID->setClipToAxisRect(false);
    labelPID->setFont(QFont("Arial", 10));
    labelPID->setColor(Qt::black);
    labelPID->setText("Wykres składowych PID");

    QVector<QCustomPlot*> plots = {ui->plotMain, ui->plotError, ui->plotControl, ui->plotPID};
    for(auto plot : plots) {
        plot->xAxis->setLabel("Czas [s]");
        plot->setInteraction(QCP::iRangeDrag, true);
        plot->setInteraction(QCP::iRangeZoom, true);
        plot->xAxis->setLabelPadding(0);
        plot->xAxis->setTickLabelPadding(2);
        plot->plotLayout()->setMargins(QMargins(0,0,0,0));
    }
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
    for(auto plot : plots) {
        for(int i=0; i<plot->graphCount(); ++i) {
            plot->graph(i)->data()->clear();
        }
        plot->replot();
    }
}

void MainWindow::onNoweData(double t, double y_zad, double y, double u, double e,
                            RegulatorPID::Skladowe skladowe)
{
    updatePlots(t, y_zad, y, u, e, skladowe);
}

void MainWindow::updatePidParams()
{
    double Kp = ui->spinKp->value();
    double Ti = ui->spinTi->value();
    double Td = ui->spinTd->value();
    double Tp = ui->spinTp->value() / 1000.0;
    manager->setNastawyPID(Kp, Ti, Td, -10.0, 10.0, Tp);
}

void MainWindow::updateGeneratorParams()
{
    double amp = ui->spinAmp->value();
    double per = ui->spinPeriod->value();
    double fill = ui->spinFill->value();
    double offset = ui->spinOffset->value();

    if(ui->comboGenType->currentIndex() == 1) {
        manager->setTypGeneratora(MenadzerSymulacji:: TypGeneratora::Sinusoidalny);
        manager->setParametryGeneratoraSinusoidalnego(amp, per, offset);
    } else {
        manager->setTypGeneratora(MenadzerSymulacji:: TypGeneratora::Prostokatny);
        manager->setParametryGeneratoraProstokatnego(amp, per, fill, offset);
    }
}

void MainWindow::updatePlots(double t, double y_zad, double y, double u, double e, RegulatorPID::Skladowe pid)
{

    ui->plotMain->graph(0)->addData(t, y_zad);
    ui->plotMain->graph(1)->addData(t, y);

    ui->plotError->graph(0)->addData(t, e);

    ui->plotControl->graph(0)->addData(t, u);

    ui->plotPID->graph(0)->addData(t, pid.P);
    ui->plotPID->graph(1)->addData(t, pid.I);
    ui->plotPID->graph(2)->addData(t, pid.D);

    double window = ui->spinWindow->value();

    QVector<QCustomPlot*> plots = {ui->plotMain, ui->plotError, ui->plotControl, ui->plotPID};

    for(auto plot : plots) {
        plot->xAxis->setRange(t, window, Qt::AlignRight);

        for(int i=0; i<plot->graphCount(); ++i) {
            plot->graph(i)->data()->removeBefore(t - window - 1.0);
        }

        plot->yAxis->rescale();

        QCPRange range = plot->yAxis->range();
        double center = range.center();
        double size = range.size();

        if (size == 0.0) size = 1.0;

        plot->yAxis->setRange(center - size * 0.6, center + size * 0.6);

        plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void MainWindow::on_btnUpdatePID_clicked()
{
    double Kp = ui->spinKp->value();
    double Ti = ui->spinTi->value();
    double Td = ui->spinTd->value();
    double Tp = ui->spinTp->value() / 1000.0;

    manager->setNastawyPID(Kp, Ti, Td, -10.0, 10.0, Tp);
}

void MainWindow::onIntervalChanged()
{
    int intervalMs = ui->spinTp->value();
    manager->getZarzadzanieCzasem()->setInterwalMs(intervalMs);

    double Tp = intervalMs / 1000.0;
    double Kp = ui->spinKp->value();
    double Ti = ui->spinTi->value();
    double Td = ui->spinTd->value();
    manager->setNastawyPID(Kp, Ti, Td, -10.0, 10.0, Tp);
}

void MainWindow::on_btnOpenARX_clicked()
{
    arxDialog->setOgraniczenia(
        manager->getOgraniczeniaARX(),
        manager->getUMinARX(),
        manager->getUMaxARX(),
        manager->getYMinARX(),
        manager->getYMaxARX()
        );

    if (arxDialog->exec() == QDialog::Accepted) {
        manager->setParametryARX(
            arxDialog->getA(),
            arxDialog->getB(),
            arxDialog->getK(),
            arxDialog->getPozSz()
            );

        manager->setOgraniczeniaARX(arxDialog->getOgraniczeniaWlaczone());
        manager->setOgraniczeniaSterowania(arxDialog->getUMin(), arxDialog->getUMax());
        manager->setOgraniczeniaRegulowanej(arxDialog->getYMin(), arxDialog->getYMax());
    }
}

void MainWindow::onWindowChanged()
{

}

void MainWindow::on_btnSave_clicked()
{
    json j;
    j["PID"] = {
        {"Kp", ui->spinKp->value()},
        {"Ti", ui->spinTi->value()},
        {"Td", ui->spinTd->value()},
        {"Tp", ui->spinTp->value()}
    };

    j["ARX"] = {
        {"k", arxDialog->getK()},
        {"noise", arxDialog->getPozSz()},
        {"A", arxDialog->getA()},
        {"B", arxDialog->getB()},
        {"ograniczenia_wlaczone", arxDialog->getOgraniczeniaWlaczone()},
        {"u_min", arxDialog->getUMin()},
        {"u_max", arxDialog->getUMax()},
        {"y_min", arxDialog->getYMin()},
        {"y_max", arxDialog->getYMax()}
    };

    j["Gen"] = {
        {"Type", ui->comboGenType->currentIndex()},
        {"Amp", ui->spinAmp->value()},
        {"Per", ui->spinPeriod->value()},
        {"Fill", ui->spinFill->value()},
        {"Offset", ui->spinOffset->value()}
    };


    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz konfigurację", "", "JSON (*.json)");
    if(fileName.isEmpty()) return;

    std::ofstream o(fileName.toStdString());
    o << j.dump(4);
}

void MainWindow::on_btnLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Wczytaj konfigurację", "", "JSON (*.json)");
    if(fileName.isEmpty()) return;

    std::ifstream i(fileName.toStdString());
    json j;
    if(i >> j) {
        ui->spinKp->setValue(j["PID"]["Kp"]);
        ui->spinTi->setValue(j["PID"]["Ti"]);
        ui->spinTd->setValue(j["PID"]["Td"]);
        ui->spinTp->setValue(j["PID"]["Tp"]);

        ui->comboGenType->setCurrentIndex(j["Gen"]["Type"]);
        ui->spinAmp->setValue(j["Gen"]["Amp"]);
        ui->spinPeriod->setValue(j["Gen"]["Per"]);
        ui->spinFill->setValue(j["Gen"]["Fill"]);
        ui->spinOffset->setValue(j["Gen"]["Offset"]);

        arxDialog->setParams(
            j["ARX"]["A"].get<std::vector<double>>(),
            j["ARX"]["B"].get<std:: vector<double>>(),
            j["ARX"]["k"],
            j["ARX"]["noise"]
            );

        if (j["ARX"]. contains("ograniczenia_wlaczone")) {
            arxDialog->setOgraniczenia(
                j["ARX"]["ograniczenia_wlaczone"],
                j["ARX"]["u_min"],
                j["ARX"]["u_max"],
                j["ARX"]["y_min"],
                j["ARX"]["y_max"]
                );
        }

        manager->setParametryARX(arxDialog->getA(), arxDialog->getB(), j["ARX"]["k"], j["ARX"]["noise"]);

        if (j["ARX"].contains("ograniczenia_wlaczone")) {
            manager->setOgraniczeniaARX(j["ARX"]["ograniczenia_wlaczone"]);
            manager->setOgraniczeniaSterowania(j["ARX"]["u_min"], j["ARX"]["u_max"]);
            manager->setOgraniczeniaRegulowanej(j["ARX"]["y_min"], j["ARX"]["y_max"]);
        }
    }
}

void MainWindow::on_btnResetIntegral_clicked()
{
    manager->resetCalkiPID();
}

void MainWindow::on_comboPidType_currentIndexChanged(int index)
{
    if(index == 0) manager->setLiczCalkPID(RegulatorPID::LiczCalk::Zew);
    else manager->setLiczCalkPID(RegulatorPID::LiczCalk::Wew);
}

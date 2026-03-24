#include "dialogarx.h"
#include "ui_dialogarx.h"

DialogARX::DialogARX(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogARX)
{
    ui->setupUi(this);

    ui->spinA1->setValue(-0.4);
    ui->spinA2->setValue(0.0);
    ui->spinA3->setValue(0.0);

    ui->spinB1->setValue(0.6);
    ui->spinB2->setValue(0.0);
    ui->spinB3->setValue(0.0);

    ui->spinK->setValue(1);
    ui->spinPozSz->setValue(0.0);

    ui->checkOgraniczenia->setChecked(true);
    ui->spinUMin->setValue(-10.0);
    ui->spinUMax->setValue(10.0);
    ui->spinYMin->setValue(-10.0);
    ui->spinYMax->setValue(10.0);

    connect(ui->btnOk, &QPushButton::clicked, this, &DialogARX:: accept);
    connect(ui->btnCancel, &QPushButton::clicked, this, &DialogARX::reject);
}

DialogARX::~DialogARX()
{
    delete ui;
}

std::vector<double> DialogARX::getA() const
{
    std::vector<double> A;

    double a1 = ui->spinA1->value();
    double a2 = ui->spinA2->value();
    double a3 = ui->spinA3->value();

    A.push_back(a1);

    if (std::abs(a2) > 1e-9 || std::abs(a3) > 1e-9) {
        A.push_back(a2);
    }

    if (std::abs(a3) > 1e-9) {
        A.push_back(a3);
    }

    return A;
}

std:: vector<double> DialogARX:: getB() const
{
    std::vector<double> B;

    double b1 = ui->spinB1->value();
    double b2 = ui->spinB2->value();
    double b3 = ui->spinB3->value();

    B.push_back(b1);

    if (std::abs(b2) > 1e-9 || std::abs(b3) > 1e-9) {
        B.push_back(b2);
    }

    if (std::abs(b3) > 1e-9) {
        B.push_back(b3);
    }

    return B;
}

int DialogARX::getK() const {
    return ui->spinK->value();
}

double DialogARX::getPozSz() const {
    return ui->spinPozSz->value();
}

bool DialogARX::getOgraniczeniaWlaczone() const {
    return ui->checkOgraniczenia->isChecked();
}

double DialogARX::getUMin() const {
    return ui->spinUMin->value();
}

double DialogARX::getUMax() const {
    return ui->spinUMax->value();
}

double DialogARX:: getYMin() const {
    return ui->spinYMin->value();
}

double DialogARX::getYMax() const {
    return ui->spinYMax->value();
}

void DialogARX::setParams(const std::vector<double>& A, const std::vector<double>& B, int k, double pozsz) {
    ui->spinA1->setValue(A. size() > 0 ? A[0] : 0.0);
    ui->spinA2->setValue(A. size() > 1 ? A[1] : 0.0);
    ui->spinA3->setValue(A.size() > 2 ? A[2] : 0.0);

    ui->spinB1->setValue(B.size() > 0 ? B[0] : 0.0);
    ui->spinB2->setValue(B.size() > 1 ? B[1] : 0.0);
    ui->spinB3->setValue(B.size() > 2 ? B[2] : 0.0);
    ui->spinK->setValue(k);
    ui->spinPozSz->setValue(pozsz);
}

void DialogARX::setOgraniczenia(bool wlaczone, double u_min, double u_max, double y_min, double y_max) {
    ui->checkOgraniczenia->setChecked(wlaczone);
    ui->spinUMin->setValue(u_min);
    ui->spinUMax->setValue(u_max);
    ui->spinYMin->setValue(y_min);
    ui->spinYMax->setValue(y_max);
}

std::vector<double> DialogARX::parseVector(QString txt) const {
    std::vector<double> vec;
    QStringList parts = txt.split(",");
    for(const auto& p : parts) {
        bool ok;
        double v = p.trimmed().toDouble(&ok);
        if(ok) vec.push_back(v);
    }
    return vec;
}

QString DialogARX::vectorToString(const std::vector<double>& v) const {
    QStringList sl;
    for(double d :  v) sl << QString::number(d);
    return sl.join(", ");
}

void DialogARX::on_btnCancel_clicked()
{
    reject();
}

void DialogARX::on_btnOk_clicked()
{
    accept();
}

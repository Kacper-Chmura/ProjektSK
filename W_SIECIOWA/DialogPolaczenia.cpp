#include "DialogPolaczenia.h"
#include "ui_DialogPolaczenia.h"

DialogPolaczenia::DialogPolaczenia(QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogPolaczenia)
{
    ui->setupUi(this);
    setModal(true);

    connect(ui->radioSerwer, &QRadioButton::toggled, this, &DialogPolaczenia::on_radioSerwer_toggled);
    //connect(ui->btnPotwierdz, &QPushButton::clicked, this, &DialogPolaczenia::on_btnPotwierdz_clicked);
    //connect(ui->btnAnuluj,    &QPushButton::clicked, this, &DialogPolaczenia::on_btnAnuluj_clicked);
}


DialogPolaczenia::~DialogPolaczenia()
{
    delete ui;
}

void DialogPolaczenia::on_radioSerwer_toggled(bool checked)
{
    ui->labelIP->setEnabled(!checked);
    ui->editIP->setEnabled(!checked);
}

void DialogPolaczenia::on_btnPotwierdz_clicked()
{
    accept();
}

void DialogPolaczenia::on_btnAnuluj_clicked()
{
    reject();
}

KonfiguracjaPolaczenia DialogPolaczenia::getKonfiguracja() const
{
    KonfiguracjaPolaczenia k;
    k.czyJestSerwer = ui->radioSerwer->isChecked();
    k.port          = ui->spinPort->value();
    k.adresIP       = ui->editIP->text().trimmed();
    k.rola          = ui->radioRegulator->isChecked() ? 0 : 1;
    return k;
}

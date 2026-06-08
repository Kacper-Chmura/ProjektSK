#include "DialogPolaczenia.h"
#include "qhostaddress.h"
#include "ui_DialogPolaczenia.h"
#include <QNetworkInterface>
#include <QHostAddress>

DialogPolaczenia::DialogPolaczenia(QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogPolaczenia)
{
    ui->setupUi(this);
    setModal(true);

    connect(ui->radioSerwer, &QRadioButton::toggled, this, &DialogPolaczenia::on_radioSerwer_toggled);
    // Inicjalizacja przy uruchomieniu okna:
    // Skoro w UI "Serwer" jest domyślnie zaznaczony, od razu wstawiamy lokalne IP
    if (ui->radioSerwer->isChecked()) {
        ui->editIP->setText(pobierzLokalnyIP());
        ui->editIP->setEnabled(false);
        ui->labelIP->setEnabled(false);
    }
}


DialogPolaczenia::~DialogPolaczenia()
{
    delete ui;
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

// ---- Implementacja metody pobierającej IP ----
QString DialogPolaczenia::pobierzLokalnyIP()
{
    QString localIp = "127.0.0.1"; // Wartość domyślna

    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (const QHostAddress &ip : ipAddressesList) {
        if (ip.protocol() == QAbstractSocket::IPv4Protocol && !ip.isLoopback()) {
            localIp = ip.toString();
            break; // Zwraca pierwszy aktywny adres IPv4 w sieci LAN
        }
    }
    return localIp;
}

// ---- Sloty przełączające tryby ----

void DialogPolaczenia::on_radioSerwer_toggled(bool checked)
{
    if (checked) {
        // Tryb SERWER: wpisujemy lokalne IP maszyny i blokujemy edycję
        ui->editIP->setText(pobierzLokalnyIP());
        ui->editIP->setEnabled(false);
        ui->labelIP->setEnabled(false);
    }
}

void DialogPolaczenia::on_radioKlient_toggled(bool checked)
{
    if (checked) {
        // Tryb KLIENT: ustawiamy domyślne localhost i odblokowujemy edycję,
        // aby użytkownik mógł wpisać IP serwera
        ui->editIP->setText("127.0.0.1");
        ui->editIP->setEnabled(true);
        ui->labelIP->setEnabled(true);
    }
}



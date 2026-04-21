#ifndef DIALOGPOLACZENIA_H
#define DIALOGPOLACZENIA_H

#include <QDialog>
#include <QString>

namespace Ui { class DialogPolaczenia; }

// Wynik dialogu
struct KonfiguracjaPolaczenia {
    bool  czyJestSerwer = true;
    int   port          = 12345;
    QString adresIP     = "127.0.0.1";
    int   rola          = 0;   // 0 = Regulator, 1 = Obiekt
};

class DialogPolaczenia : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPolaczenia(QWidget* parent = nullptr);
    ~DialogPolaczenia() override;

    KonfiguracjaPolaczenia getKonfiguracja() const;

private slots:
    void on_radioSerwer_toggled(bool checked);
    void on_btnPotwierdz_clicked();
    void on_btnAnuluj_clicked();

private:
    Ui::DialogPolaczenia* ui;
};

#endif // DIALOGPOLACZENIA_H

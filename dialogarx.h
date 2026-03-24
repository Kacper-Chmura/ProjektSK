#pragma once

#include <QDialog>
#include <vector>

namespace Ui {
class DialogARX;
}

class DialogARX : public QDialog
{
    Q_OBJECT

public:
    explicit DialogARX(QWidget *parent = nullptr);
    ~DialogARX();

    std::vector<double> getA() const;
    std::vector<double> getB() const;
    int getK() const;
    double getPozSz() const;

    bool getOgraniczeniaWlaczone() const;
    double getUMin() const;
    double getUMax() const;
    double getYMin() const;
    double getYMax() const;

    void setParams(const std::vector<double>& A, const std::vector<double>& B, int k, double pozSz);
    void setOgraniczenia(bool wlaczone, double uMin, double uMax, double yMin, double yMax);

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DialogARX *ui;

    std::vector<double> parseVector(QString txt) const;
    QString vectorToString(const std::vector<double>& v) const;
};

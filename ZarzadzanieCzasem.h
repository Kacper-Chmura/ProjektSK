#pragma once
#include <QObject>
#include <QTimer>

class MenadzerSymulacji;

class ZarzadzanieCzasem : public QObject
{
    Q_OBJECT

public:
    explicit ZarzadzanieCzasem(MenadzerSymulacji* manager, QObject *parent = nullptr);

    void setInterwalMs(int ms);
    int getInterwalMs() const { return _intervalMs; }
    double getInterwalSek() const { return _intervalMs / 1000.0; }
    double getCzasSymulacji() const { return _czasSymulacji; }

    void start();
    void stop();
    void reset();
    bool czyUruchomiony() const;

private slots:
    void onTimeout();

private:
    QTimer* _timer;
    int _intervalMs;
    double _czasSymulacji;
    MenadzerSymulacji* _manager;
};

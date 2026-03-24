#include "ZarzadzanieCzasem.h"
#include "MenadzerSymulacji.h"

ZarzadzanieCzasem::ZarzadzanieCzasem(MenadzerSymulacji* manager, QObject *parent)
    : QObject(parent)
    , _timer(new QTimer(this))
    , _intervalMs(200)
    , _czasSymulacji(0.0)
    , _manager(manager)
{
    _timer->setTimerType(Qt::CoarseTimer);
    connect(_timer, &QTimer::timeout, this, &ZarzadzanieCzasem::onTimeout);
}

void ZarzadzanieCzasem::setInterwalMs(int ms)
{
    if (ms < 10 || ms > 1000) return;
    _intervalMs = ms;
    if (_timer->isActive()) {
        _timer->stop();
        _timer->start(ms);
    }
}

void ZarzadzanieCzasem::start()
{
    _timer->start(_intervalMs);
}

void ZarzadzanieCzasem::stop()
{
    _timer->stop();
}

void ZarzadzanieCzasem::reset()
{
    _timer->stop();
    _czasSymulacji = 0.0;
}

bool ZarzadzanieCzasem::czyUruchomiony() const
{
    return _timer->isActive();
}

void ZarzadzanieCzasem::onTimeout()
{
    _czasSymulacji += getInterwalSek();
    _manager->wykonajKrokSymulacji(_czasSymulacji);
}

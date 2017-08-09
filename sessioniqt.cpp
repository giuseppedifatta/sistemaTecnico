#include "sessioniqt.h"

SessioniQt::SessioniQt(QDateTime inizio, QDateTime fine)
{
    this->inizio = inizio;
    this->fine = fine;
}

QDateTime SessioniQt::getInizio() const
{
    return inizio;
}

void SessioniQt::setInizio(const QDateTime &value)
{
    inizio = value;
}

QDateTime SessioniQt::getFine() const
{
    return fine;
}

void SessioniQt::setFine(const QDateTime &value)
{
    fine = value;
}


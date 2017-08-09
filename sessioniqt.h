#ifndef SESSIONIQT_H
#define SESSIONIQT_H
#include <QDateTime>

class SessioniQt
{
public:
    SessioniQt(QDateTime inizio, QDateTime fine);
    QDateTime getInizio() const;
    void setInizio(const QDateTime &value);

    QDateTime getFine() const;
    void setFine(const QDateTime &value);

private:
    QDateTime inizio;
    QDateTime fine;
};

#endif // SESSIONIQT_H

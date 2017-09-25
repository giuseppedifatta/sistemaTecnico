#ifndef INFOSEGGIO_H
#define INFOSEGGIO_H

#include <string>
#include <QtCore>

using namespace std;

class InfoSeggio
{
public:
    InfoSeggio();
    string getDescrizione() const;
    void setDescrizione(const string &value);

    string getIp() const;
    void setIp(const string &value);

    uint getIdSeggio() const;
    void setIdSeggio(const uint &value);

private:
    uint idSeggio;
    string descrizione;
    string ip;
};
Q_DECLARE_METATYPE(InfoSeggio)
#endif // INFOSEGGIO_H

#include "infoseggio.h"

InfoSeggio::InfoSeggio()
{
    idSeggio = 0;
}

string InfoSeggio::getDescrizione() const
{
    return descrizione;
}

void InfoSeggio::setDescrizione(const string &value)
{
    descrizione = value;
}

string InfoSeggio::getIp() const
{
    return ip;
}

void InfoSeggio::setIp(const string &value)
{
    ip = value;
}

uint InfoSeggio::getIdSeggio() const
{
    return idSeggio;
}

void InfoSeggio::setIdSeggio(const uint &value)
{
    idSeggio = value;
}


#include "chiavesessione.h"

string ChiaveSessione::getSharedKey() const
{
    return sharedKey;
}

void ChiaveSessione::setSharedKey(const string &value)
{
    sharedKey = value;
}

string ChiaveSessione::getIpPostazione() const
{
    return ipPostazione;
}

void ChiaveSessione::setIpPostazione(const string &value)
{
    ipPostazione = value;
}

uint ChiaveSessione::getIdSeggio() const
{
    return idSeggio;
}

void ChiaveSessione::setIdSeggio(const uint &value)
{
    idSeggio = value;
}

SessioneVoto ChiaveSessione::getSessioneVoto() const
{
    return sessioneVoto;
}

void ChiaveSessione::setSessioneVoto(const SessioneVoto &value)
{
    sessioneVoto = value;
}

string ChiaveSessione::getSedeSeggio() const
{
    return sedeSeggio;
}

void ChiaveSessione::setSedeSeggio(const string &value)
{
    sedeSeggio = value;
}

ChiaveSessione::ChiaveSessione()
{
    
}

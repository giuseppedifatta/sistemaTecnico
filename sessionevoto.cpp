#include "sessionevoto.h"

SessioneVoto::SessioneVoto()
{

}

string SessioneVoto::getOraApertura() const
{
    return oraApertura;
}

void SessioneVoto::setOraApertura(const string &value)
{
    oraApertura = value;
}

string SessioneVoto::getOraChiusura() const
{
    return oraChiusura;
}

void SessioneVoto::setOraChiusura(const string &value)
{
    oraChiusura = value;
}

string SessioneVoto::getData() const
{
    return data;
}

void SessioneVoto::setData(const string &value)
{
    data = value;
}




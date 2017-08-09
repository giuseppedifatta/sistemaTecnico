#include "responsabileprocedimento.h"

ResponsabileProcedimento::ResponsabileProcedimento()
{

}

string ResponsabileProcedimento::getNome() const
{
    return nome;
}

void ResponsabileProcedimento::setNome(const string &value)
{
    nome = value;
}

string ResponsabileProcedimento::getCognome() const
{
    return cognome;
}

void ResponsabileProcedimento::setCognome(const string &value)
{
    cognome = value;
}

string ResponsabileProcedimento::getLuogoNascita() const
{
    return luogoNascita;
}

void ResponsabileProcedimento::setLuogoNascita(const string &value)
{
    luogoNascita = value;
}

string ResponsabileProcedimento::getDataNascita() const
{
    return dataNascita;
}

void ResponsabileProcedimento::setDataNascita(const string &value)
{
    dataNascita = value;
}

string ResponsabileProcedimento::getUserid() const
{
    return userid;
}

void ResponsabileProcedimento::setUserid(const string &value)
{
    userid = value;
}

void ResponsabileProcedimento::setPassword(const string &value)
{
    password = value;
}

uint ResponsabileProcedimento::getIdRP() const
{
    return idRP;
}

void ResponsabileProcedimento::setIdRP(const uint &value)
{
    idRP = value;
}

string ResponsabileProcedimento::getPassword() const
{
    return password;
}


#include "tipovotante.h"

TipoVotante::TipoVotante()
{

}

unsigned int TipoVotante::getId() const
{
    return id;
}

void TipoVotante::setId(unsigned int value)
{
    id = value;
}

string TipoVotante::getDescrizione() const
{
    return descrizione;
}

void TipoVotante::setDescrizione(const string &value)
{
    descrizione = value;
}


#include "candidato.h"

Candidato::Candidato(std::__cxx11::string nominativo, std::__cxx11::string as_g)
{
    this->nominativo=nominativo;
    this->as_g=as_g;
}

std::string Candidato::getNominativo() const
{
    return nominativo;
}

void Candidato::setNominativo(const std::string &value)
{
    nominativo = value;
}

std::string Candidato::getAs_g() const
{
    return as_g;
}

void Candidato::setAs_g(const std::string &value)
{
    as_g = value;
}


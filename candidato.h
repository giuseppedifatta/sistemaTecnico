#ifndef CANDIDATO_H
#define CANDIDATO_H


#include <string>

class Candidato
{
public:
    Candidato();
    std::string getNominativo() const;
    void setNominativo(const std::string &value);

    std::string getAs_g() const;
    void setAs_g(const std::string &value);

private:
    std::string as_g;
    std::string nominativo;

};

#endif // CANDIDATO_H

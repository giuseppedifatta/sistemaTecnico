#ifndef SCHEDAVOTO_H
#define SCHEDAVOTO_H
#include <vector>
#include <string>
#include "candidato.h"

class SchedaVoto
{
public:
    SchedaVoto();

private:
    unsigned int id;
    unsigned int numPreferenze;
    std::vector <std::string> as_g;
    std::vector <Candidato> candidati;
    std::vector <unsigned int> classiElettoratoAttivo;

    enum tipologieElettoratoAttivo{
        studente,
        professore,
        amministrativo,
        senatoAccademico
    };

    //e se fosse il sistema a decidere le tipologie di elettorato attivo?
    //basandosi sulla tipologia di elettorato Passivo
    enum tipologieElettoratoPassivo{
        rettore,
        senatoAccademico,
        consiglioCDL
    };


};

#endif // SCHEDAVOTO_H

#ifndef SCHEDAVOTO_H
#define SCHEDAVOTO_H
#include <vector>
#include <string>
#include "candidato.h"
#include <QtCore>

class SchedaVoto{
    //la scheda di voto viene costruita e riempita tramite l'interfaccia grafica,
    //ma memorizzata sul model solo una volta che sarà cliccato il tasto di completamento della scheda,
    //diversamente i dati inseriti verranno scartati
public:
    //SchedaVoto();
    SchedaVoto();

    unsigned int getId() const;
    void setId(unsigned int value);

    unsigned int getNumPreferenze() const;
    void setNumPreferenze(unsigned int value);
    void addLista(std::string lista);
    void removeLista(uint index);

    void removeCandidato(uint index);

    unsigned int getModalitaAdd() const;
    void setModalitaAdd(unsigned int value);

    enum tipologieElettoratoAttivo{
        studente,
        professore,
        amministrativo,
        senatoAccademico
    };

    enum modoAdd{
        candidato,
        lista
    };

    //e se fosse il sistema a decidere le tipologie di elettorato attivo?
    //basandosi sulla tipologia di elettorato Passivo
    enum tipologieElezioni{
        SA, //Senato Accademico
        CA, //Consiglio di Amministrazione
        TA, //Rappresentanti del Personale Tecnico Amministrativo
        CCDS, //Consiglio di Corso di Studio
        CI, //Consiglio di Interclasse
        CD, //Consiglio di Dipartimento

    };


    std::vector<Candidato> getListCandidati() const;

    unsigned int getIdProceduraVoto() const;
    void setIdProceduraVoto(unsigned int value);

    void addCandidato(std::string lista, std::string nome, std::string cognome, std::string data, std::string luogo);
    std::vector<std::string> getListListe() const;
    void setListListe(const std::vector<std::string> &value);

    unsigned int getTipoElezione() const;
    void setTipoElezione(unsigned int value);

private:
    unsigned int idProceduraVoto;
    unsigned int id;
    unsigned int numPreferenze;
    unsigned int modalitaAdd;

    //AssociazioneStudentesca_Gruppo
    std::vector<std::string> listListe;

    std::vector <Candidato> listCandidati;

    unsigned int tipoElezione;




};

Q_DECLARE_METATYPE(SchedaVoto)
#endif // SCHEDAVOTO_H

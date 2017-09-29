#ifndef SCHEDAVOTO_H
#define SCHEDAVOTO_H
#include <vector>
#include <string>
#include "candidato.h"
#include "listaelettorale.h"
#include <QtCore>

class SchedaVoto{
    //la scheda di voto viene costruita e riempita tramite l'interfaccia grafica,
    //ma memorizzata sul model solo una volta che sarà cliccato il tasto di completamento della scheda,
    //diversamente i dati inseriti verranno scartati
public:

    SchedaVoto();

    unsigned int getId() const;
    void setId(unsigned int value);

    unsigned int getNumPreferenze() const;
    void setNumPreferenze(unsigned int value);
    void addLista(std::string nomeLista);
    vector<Candidato> removeLista(uint index);

    void removeCandidatoFromLista(uint index);

    unsigned int getModalitaAdd() const;
    void setModalitaAdd(unsigned int value);


    enum modoAdd{
        candidato,
        lista
    };



    std::vector<Candidato> getCandidati() const;

    unsigned int getIdProceduraVoto() const;
    void setIdProceduraVoto(unsigned int value);

    bool addCandidato(string matricola, string nome, string cognome ="",string lista = "", string data ="", string luogo="");
//    std::vector<std::string> getListListe() const;
//    void setListListe(const std::vector<std::string> &value);


    std::vector<ListaElettorale> getListeElettorali() const;
    //void setListeElettorali(const std::vector<ListaElettorale> &value);

    void removeCandidatiFromScheda(vector<Candidato> &candidatiDaRimuovere);
    bool soloCandidatiMode() const;
    void soloCandidatiOn();
    void soloCandidatiOff();

    std::vector<uint> getIdTipiVotantiConsentiti() const;
    void setIdTipiVotantiConsentiti(const std::vector<uint> &value);
    void addIdTipiVotantiConsentiti(uint id);

    string getDescrizioneElezione() const;
    void setDescrizioneElezione(const string &value);

private:
    unsigned int idProceduraVoto;
    unsigned int id;
    unsigned int numPreferenze;
    unsigned int modalitaAdd;
    unsigned int numeroLista;
    //liste elettorali
    std::vector <ListaElettorale> listeElettorali;

    //candidati
    std::vector <Candidato> candidati;

    string descrizioneElezione;
    std::vector <uint> idTipiVotantiConsentiti;
    bool soloCandidati;

};

Q_DECLARE_METATYPE(SchedaVoto)
#endif // SCHEDAVOTO_H

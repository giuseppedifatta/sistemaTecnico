#include "schedavoto.h"
#include <iostream>
using namespace std;
SchedaVoto::SchedaVoto()
{
    modalitaAdd=SchedaVoto::modoAdd::candidato;

    //id della procedura a cui la scheda di voto è associata

}

unsigned int SchedaVoto::getId() const
{
    return id;
}

void SchedaVoto::setId(unsigned int value)
{
    id = value;
}

unsigned int SchedaVoto::getNumPreferenze() const
{
    return numPreferenze;
}

void SchedaVoto::setNumPreferenze(unsigned int value)
{
    numPreferenze = value;
}

void SchedaVoto::addLista(std::string lista)
{
    this->listListe.push_back(lista);
}

void SchedaVoto::removeLista(uint index)
{
    std::string gruppo = this->listListe.at(index);
    for (uint i = 0; i < listCandidati.size(); i++){
        if(listCandidati.at(i).getLista() == gruppo){
            cout << listCandidati.at(i).getLista();
            removeCandidato(i);
            //riprendi la scansione dall'elemento precedente, poichè il vettore avrà un elemento in meno dopo la rimozione dell'elemento di posizione i
            i--;

        }
    }

    this->listListe.erase(listListe.begin()+index);
}

void SchedaVoto::addCandidato(std::string nome, std::string lista = "", std::string cognome ="", std::string data ="", std::string luogo="")
{
    Candidato c(nome,lista,cognome, data, luogo);
    this->listCandidati.push_back(c);

}

std::vector<std::string> SchedaVoto::getListListe() const
{
    return listListe;
}

void SchedaVoto::setListListe(const std::vector<std::string> &value)
{
    listListe = value;
}

unsigned int SchedaVoto::getTipoElezione() const
{
    return tipoElezione;
}

void SchedaVoto::setTipoElezione(unsigned int value)
{
    tipoElezione = value;
}

void SchedaVoto::removeCandidato(uint index)
{
    std::string s = listCandidati.at(index).getNome();
    cout << "Rimuovo il candidato: " << s << endl;
    this->listCandidati.erase(listCandidati.begin()+index);
}

unsigned int SchedaVoto::getModalitaAdd() const
{
    return modalitaAdd;
}

void SchedaVoto::setModalitaAdd(unsigned int value)
{
    modalitaAdd = value;
}

std::vector<Candidato> SchedaVoto::getListCandidati() const
{
    return listCandidati;
}

unsigned int SchedaVoto::getIdProceduraVoto() const
{
    return idProceduraVoto;
}

void SchedaVoto::setIdProceduraVoto(unsigned int value)
{
    idProceduraVoto = value;
}



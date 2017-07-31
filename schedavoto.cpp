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

void SchedaVoto::addAs_g(std::string as_g)
{
    this->listAs_g.push_back(as_g);
}

void SchedaVoto::removeAs_g(uint index)
{
    std::string gruppo = this->listAs_g.at(index);
    for (uint i = 0; i < listCandidati.size(); i++){
        if(listCandidati[i].getAs_g() == gruppo){
            cout << listCandidati[i].getAs_g();
            removeCandidato(i);
            //riprendi la scansione dall'elemento precedente, poichè il vettore avrà un elemento in meno dopo la rimozione dell'elemento di posizione i
            i--;

        }
    }

    this->listAs_g.erase(listAs_g.begin()+index);
}

void SchedaVoto::addCandidato(std::string nome, std::string lista = "", std::string cognome ="", std::string data ="", std::string luogo="")
{
    Candidato c(nome,lista);
    this->listCandidati.push_back(c);

}

void SchedaVoto::removeCandidato(uint index)
{
    std::string s = listCandidati.at(index).getNominativo();
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

std::vector<std::string> SchedaVoto::getListAs_g() const
{
    return listAs_g;
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



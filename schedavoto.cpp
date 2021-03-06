#include "schedavoto.h"
#include <iostream>
using namespace std;
SchedaVoto::SchedaVoto()
{
    modalitaAdd = SchedaVoto::modoAdd::candidato;
    numeroLista = 0;
    numPreferenze = 0;
    idProceduraVoto = 0;

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

void SchedaVoto::addLista(std::string nomeLista)
{
    numeroLista++;
    uint numLista = numeroLista;
    ListaElettorale l(nomeLista, numLista);
    this->listeElettorali.push_back(l);
}

vector <Candidato> SchedaVoto::removeLista(uint index)
{

    //restituisce il vettore dei candidati da rimuovere dalla struttura schedaVoto
    ListaElettorale listaToRemove  = listeElettorali.at(index);

    listeElettorali.erase(listeElettorali.begin()+index);
    return listaToRemove.getCandidati();
}





std::vector<ListaElettorale> SchedaVoto::getListeElettorali() const
{
    return listeElettorali;
}


bool SchedaVoto::addCandidato
(string matricola, string nome, string cognome,string lista, string data, string luogo)
{
    //creo il candidato
    Candidato c(nome,lista,cognome,data,luogo,matricola);

    //verifico che non sia presente un candidato con la stessa matricola
    for(unsigned int i = 0; i < candidati.size(); i++){
        if(candidati.at(i).getMatricola() == c.getMatricola()){
            cout << "candidato con la matricola inserita già presente" << endl;
            return false;
        }
    }


    bool listaTrovata = false;
    //se la lista esiste già, aggiungo candidato alla sua lista
    for(unsigned int index = 0; index < listeElettorali.size(); index ++){
        if (listeElettorali.at(index).getNome() == lista){
            listaTrovata = true;
            cout << "candidato con matricola: " << matricola << ", aggiunto alla lista: " << lista << endl;
            listeElettorali.at(index).addCandidato(c);

            //dopo aver aggiunto il candidato alla struttura lista, lo aggiungo anche alla scheda
            candidati.push_back(c);

            //non eseguo il codice che segue questo for
            return true;
        }
    }

    //se la lista non esiste o è la lista senza nome e non sono ancora presenti nella scheda candidati senza lista
    //bisogna aggiungere la nuova lista e poi aggiungere il candidato alla scheda
    if(!listaTrovata){
        //aggiungo la lista
        addLista(lista);
        //aggiungo il candidato
        addCandidato(matricola,nome,cognome,lista,data,luogo);
    }
    return true;
}
void SchedaVoto::removeCandidatoFromLista(uint index)
{
    string listaAppartenenza = candidati.at(index).getLista();
    string matricola = candidati.at(index).getMatricola();

    cout << "Rimuovo il candidato con matricola: " << matricola << endl;
    this->candidati.erase(candidati.begin()+index);

    for(unsigned int index = 0; index < listeElettorali.size(); index ++){
        if (listeElettorali.at(index).getNome() == listaAppartenenza){
            listeElettorali.at(index).removeCandidato(matricola);
            return;
        }
    }
}

void SchedaVoto::removeCandidatiFromScheda(vector <Candidato> &candidatiDaRimuovere){
    cout << "Numero candidati da rimuovere: " <<  candidatiDaRimuovere.size() << endl;
    for (uint candidatoIndex = 0; candidatoIndex < candidatiDaRimuovere.size(); candidatoIndex++){
        string matrCandidatoDaRimuovere = candidatiDaRimuovere.at(candidatoIndex).getMatricola();
        for (uint index = 0; index < candidati.size();index++){
            if(candidati.at(index).getMatricola() == matrCandidatoDaRimuovere){
                candidati.erase(candidati.begin()+index);
                cout << "Rimosso candidato con matricola: " << matrCandidatoDaRimuovere<< endl;
                break;
            }
        }
    }

}

unsigned int SchedaVoto::getModalitaAdd() const
{
    return modalitaAdd;
}

void SchedaVoto::setModalitaAdd(unsigned int value)
{
    modalitaAdd = value;
}

std::vector<Candidato> SchedaVoto::getCandidati() const
{
    return candidati;
}

unsigned int SchedaVoto::getIdProceduraVoto() const
{
    return idProceduraVoto;
}

void SchedaVoto::setIdProceduraVoto(unsigned int value)
{
    idProceduraVoto = value;
}


bool SchedaVoto::soloCandidatiMode() const
{
    return soloCandidati;
}

void SchedaVoto::soloCandidatiOn()
{
    this->soloCandidati = true;
}

void SchedaVoto::soloCandidatiOff()
{
    this->soloCandidati = false;
}

std::vector<uint> SchedaVoto::getIdTipiVotantiConsentiti() const
{
    return idTipiVotantiConsentiti;
}

//void SchedaVoto::setIdTipiVotantiConsentiti(const std::vector<uint> &value)
//{
//    idTipiVotantiConsentiti = value;
//}

void SchedaVoto::addIdTipiVotantiConsentiti(uint id)
{
    idTipiVotantiConsentiti.push_back(id);
}

string SchedaVoto::getDescrizioneElezione() const
{
    return descrizioneElezione;
}

void SchedaVoto::setDescrizioneElezione(const string &value)
{
    descrizioneElezione = value;
}

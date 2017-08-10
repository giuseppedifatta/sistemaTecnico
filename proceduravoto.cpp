#include "proceduravoto.h"

ProceduraVoto::ProceduraVoto()
{
    this->data_ora_inizio = "";
    this->data_ora_termine = "";
    getRPsFromDB();
}

string ProceduraVoto::getDescrizione() const
{
    return descrizione;
}

void ProceduraVoto::setDescrizione(const string &value)
{
    descrizione = value;
}

uint ProceduraVoto::getNumSchedeVoto() const
{
    return numSchedeVoto;
}

void ProceduraVoto::setNumSchedeVoto(const uint &value)
{
    numSchedeVoto = value;
}

uint ProceduraVoto::getStato() const
{
    return stato;
}

void ProceduraVoto::setStato(const uint &value)
{
    stato = value;
}

uint ProceduraVoto::getIdRP() const
{
    return idRP;
}

void ProceduraVoto::setIdRP(const uint &value)
{
    idRP = value;
}

string ProceduraVoto::getData_ora_inizio() const
{
    return data_ora_inizio;
}

void ProceduraVoto::setData_ora_inizio(const string &value)
{
    data_ora_inizio = value;
}

string ProceduraVoto::getData_ora_termine() const
{
    return data_ora_termine;
}

void ProceduraVoto::setData_ora_termine(const string &value)
{
    data_ora_termine = value;
}

void ProceduraVoto::addSessione(SessioneVoto *sessione)
{
    sessioni.push_back(*sessione);
}

vector<SessioneVoto> ProceduraVoto::getSessioni() const
{
    return sessioni;
}

void ProceduraVoto::resetSessioni()
{
    sessioni.clear();
}

void ProceduraVoto::removeSessioneByIndex(int index)
{
    this->sessioni.erase(sessioni.begin()+index);
}

string ProceduraVoto::getInfoRP(uint idRP)
{   string infoRP;
    if(rps.empty()){
        return "";
    }
    else{
        for(unsigned int i = 0; i< rps.size(); i++){
            string nome,cognome,data,luogo;
            if(rps.at(i).getIdRP() == idRP){
                nome = rps.at(i).getNome();
                cognome = rps.at(i).getCognome();
                data = rps.at(i).getDataNascita();
                luogo = rps.at(i).getLuogoNascita();
                infoRP =  nome + " " + cognome + ", " + luogo + ", " + data;
                break;
            }
        }
    }
    return infoRP;
}

void ProceduraVoto::copyToRPS(vector<ResponsabileProcedimento> rps)
{
    copy(rps.begin(), rps.end(), this->rps.begin());
}

void ProceduraVoto::getRPsFromDB()
{

}


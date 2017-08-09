#include "datamanager.h"
#include "schedavoto.h"
#include <tinyxml2.h>
#include <sstream>

using namespace tinyxml2;

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult)
#endif

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    tecnicoPass= "tecnico";
    suPass = "admin";

    //qRegisterMetaType<SchedaVoto>("SchedaVoto");
}

void DataManager::checkPassTecnico(QString pass)
{
   if(pass==tecnicoPass){
       emit passOK();
   }
   else{
       emit wrongTecnicoPass();
   }
}

void DataManager::tryChangeTecnicoPass(QString su_pass, QString newTecnicoPass)
{
    if(su_pass==suPass){
        this->tecnicoPass = newTecnicoPass;

        emit tecnicoPassChanged();
    }
    else{
        emit wrongSUpass();
    }
}

void DataManager::storeScheda(SchedaVoto *scheda)
{
    // estrazione dati dall'oggetto di tipo scheda voto, e generazione del file XML, con conseguente memorizzazione su database

    XMLDocument xmlDoc;
    XMLNode * pRoot = xmlDoc.NewElement("SchedaVoto");
    xmlDoc.InsertFirstChild(pRoot);
    XMLElement * pElement;
//    std::ostringstream s;
//    s << idProceduraVoto;
//    const std::string i_as_string(s.str());

    uint idScheda = 1;
    pElement = xmlDoc.NewElement("idScheda");
    pElement->SetText(idScheda);
    pRoot->InsertEndChild(pElement);

    uint idProceduraVoto = scheda->getIdProceduraVoto();
    pElement = xmlDoc.NewElement("idProcedura");
    pElement->SetText(idProceduraVoto);
    pRoot->InsertEndChild(pElement);

    uint tipoElezione = scheda->getTipoElezione();
    pElement = xmlDoc.NewElement("tipologiaElezione");
    pElement->SetText(tipoElezione);
    pRoot->InsertEndChild(pElement);

    uint numPref = scheda->getNumPreferenze();
    pElement = xmlDoc.NewElement("numeroPreferenze");
    pElement->SetText(numPref);
    pRoot->InsertEndChild(pElement);


    XMLElement *pCandidati = xmlDoc.NewElement("candidati");
    pRoot->InsertEndChild(pCandidati);

    XMLElement *pCandidato;
    vector <Candidato> listCandidati = scheda->getListCandidati();
    for(uint index = 0; index < listCandidati.size(); index ++ ){
        pCandidato = xmlDoc.NewElement("candidato");
        pCandidato->SetText(listCandidati.at(index).getNome().c_str());

        //aggiungo il candidato all'insieme dei candidati
        pCandidati->InsertEndChild(pCandidato);
    }



    string nomeFile = "schedaVoto"+to_string(idScheda)+".xml";
    XMLError eResult = xmlDoc.SaveFile(nomeFile.c_str());
    if (eResult != XML_SUCCESS) {
        printf("XMLError: %i\n", eResult);
    }
    else{
        emit storedSchedaVoto();
    }

}

void DataManager::storeRP(ResponsabileProcedimento *rp)
{

}

void DataManager::storeProcedura(ProceduraVoto *procedura)
{

}


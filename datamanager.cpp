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
    uint idProceduraVoto = scheda->getIdProceduraVoto();
    XMLDocument xmlDoc;
    XMLNode * pRoot = xmlDoc.NewElement("SchedaVoto");
    xmlDoc.InsertFirstChild(pRoot);
    XMLElement * pElement;
//    std::ostringstream s;
//    s << idProceduraVoto;
//    const std::string i_as_string(s.str());
    pElement = xmlDoc.NewElement("idProcedura");
    pElement->SetText(idProceduraVoto);
    pRoot->InsertEndChild(pElement);
    pElement = xmlDoc.NewElement("tipologiaElezione");
    pElement->SetText(idProceduraVoto);
    pRoot->InsertEndChild(pElement);

    XMLError eResult = xmlDoc.SaveFile("schedaVoto.xml");
    if (eResult != XML_SUCCESS) {
        printf("XMLError: %i\n", eResult);
    }
    else{
        emit storedSchedaVoto();
    }

}


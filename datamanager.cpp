#include "datamanager.h"

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    tecnicoPass= "tecnico";
    suPass = "admin";
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


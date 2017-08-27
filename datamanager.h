#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QDateTime>
#include <tinyxml2.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>


#include "schedavoto.h"
#include "proceduravoto.h"
#include "responsabileprocedimento.h"


#include <cryptopp/pwdbased.h>
#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/secblock.h>
#include <cryptopp/hex.h>
#include <cryptopp/rsa.h>

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>

using namespace sql;
using namespace tinyxml2;
using namespace std;

class DataManager : public QObject
{
    Q_OBJECT
public:
    DataManager(QObject *parent = 0);
    ~DataManager();

signals:
    void passOK();
    void wrongTecnicoPass();
    void tecnicoPassChanged();
    void wrongSUpass();
    void storedSchedaVoto();
    void storedRP(QString userid);
    void storedProcedura();
    void readyRPS(vector <ResponsabileProcedimento> rps);
    void readyProcedure(QList <ProceduraVoto> procedure);
    void readySessioni(QList <SessioneVoto> sessioni);
    void readySchede(QList <SchedaVoto> schede);
    void deletedProcedura();
public slots:
    void checkPassTecnico(QString pass);
    void tryChangeTecnicoPass(QString su_pass, QString newTecnicoPas);
    void storeScheda(SchedaVoto* scheda);
    void storeRP(ResponsabileProcedimento* rp);
    void storeProcedura(ProceduraVoto* procedura);
    void getRPSFromDB();
    void getProcedureVotoFromDB();
    void getSessioniProceduraFromDB(uint idProcedura);
    void deleteProceduraVoto(uint idProceduraVoto);
    void getSchedeProceduraFromDB(uint idProcedura);
private:

    Driver *driver;
    Connection *connection;


    void storePassNewUser(string userid, string pass);
    string hashPassword(string plainPass, string salt);
    bool verifyUserPass(string userid, string userPass);
    void updateUserPassword(string userid, string nuovaPassword);
};

#endif // DATAMANAGER_H

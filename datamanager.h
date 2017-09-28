#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QDateTime>
#include <tinyxml2.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <time.h>

#include "schedavoto.h"
#include "proceduravoto.h"
#include "responsabileprocedimento.h"
#include "infoseggio.h"


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
using namespace CryptoPP;

class DataManager : public QObject
{
    Q_OBJECT
public:
    DataManager(QObject *parent = 0);
    ~DataManager();

    enum statoVoto{
            non_espresso,
            votando,
            espresso
        };
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
    void proceduraRangeAvailable(QDateTime inizio, QDateTime termine);
    void requestedProceduraRangeInUse();
    void readyInfoSeggi(vector<InfoSeggio> seggiPresenti);
    void idSeggioCreating(uint idSeggio);
    void abortedSeggio();
    void storedSeggio();
    void tokenStored(uint sn, string user, string pass, uint idSeggio);
    void tokenNotAvailable();
    void testTokenFail();
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
    void checkAvailabilityProceduraRange(QDateTime inizio,QDateTime termine);
    void getInfoSeggi();
    void deleteSeggio(uint idSeggio);
    void addPostazioniNoCommit(vector <string> ipPostazioni, string descrizioneSeggio);
    void rollbackSeggio();
    void commitSeggio();
    void testTokenAndStoreNoCommit(uint sn,string user,string pass,uint otp,uint idSeggio);
private:

    Driver *driver;
    Connection *connection;


    void storePassNewUser(string userid, string pass);
    string hashPassword(string plainPass, string salt);
    bool verifyUserPass(string userid, string userPass);
    void updateUserPassword(string userid, string nuovaPassword);
    string deriveKeyFromPass(string password);
    string AESencryptStdString(string plain, SecByteBlock key, byte* iv);
    string AESdecryptStdString(string encodedCipher, SecByteBlock key, byte *iv);
    string AESencryptStdString(string plain, SecByteBlock key, SecByteBlock iv);
    string AESdecryptStdString(string encodedCipher, SecByteBlock key, SecByteBlock iv);
    string currentTimeDbFormatted(string user,string pass,uint otp);
    void validateOTP(string user,string pass,uint otp);
};

#endif // DATAMANAGER_H

#include "datamanager.h"
//#include "mysql_driver.h"

//#ifndef XMLCheckResult
//#define XMLCheckResult(a_eResult)
//#endif

#include <istream>
#include <QDebug>

using namespace CryptoPP;
DataManager::DataManager(QObject *parent) : QObject(parent)
{
    tecnicoPass= "tecnico";
    suPass = "admin";
    try{
        driver=get_driver_instance();
        connection=driver->connect("localhost:3306","root", "root");
        connection->setAutoCommit(false);
        connection->setSchema("mydb");
    }catch(SQLException &ex){
        cout<<"Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    cout << "MySql Server ok." << endl;
}

DataManager::~DataManager()
{
    delete connection;
    //delete driver;
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

    //    uint idScheda = 1;
    //    pElement = xmlDoc.NewElement("idScheda");
    //    pElement->SetText(idScheda);
    //    pRoot->InsertEndChild(pElement);

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

    vector <ListaElettorale> listeInserite = scheda->getListeElettorali();
    XMLNode * pListe = xmlDoc.NewElement("liste");
    pRoot->InsertEndChild(pListe);
    unsigned int idCandidato = 0;
    unsigned int idLista = 0;
    for (uint indexListe = 0; indexListe < listeInserite.size(); indexListe++){
        idLista++;
        XMLElement * pNuovaLista = xmlDoc.NewElement("lista");
        pNuovaLista->SetAttribute("id",idLista);
        pNuovaLista->SetAttribute("nome",listeInserite.at(indexListe).getNome().c_str());

        pListe->InsertEndChild(pNuovaLista);
        vector <Candidato> candidatiLista =  listeInserite.at(indexListe).getCandidati();
        for (uint indexCandidati = 0; indexCandidati < candidatiLista.size(); indexCandidati++){

            idCandidato++;
            pElement = xmlDoc.NewElement("candidato");
            pElement->SetAttribute("id",idCandidato);
            pNuovaLista->InsertEndChild(pElement);



            XMLElement * pMatr = xmlDoc.NewElement("matricola");
            pMatr->SetText(candidatiLista.at(indexCandidati).getMatricola().c_str());
            pElement->InsertEndChild(pMatr);

            XMLElement * pNome = xmlDoc.NewElement("nome");
            pNome->SetText(candidatiLista.at(indexCandidati).getNome().c_str());
            pElement->InsertEndChild(pNome);

            XMLElement * pCognome = xmlDoc.NewElement("cognome");
            pCognome->SetText(candidatiLista.at(indexCandidati).getCognome().c_str());
            pElement->InsertEndChild(pCognome);

            XMLElement * pLN = xmlDoc.NewElement("luogoNascita");
            pLN->SetText(candidatiLista.at(indexCandidati).getLuogoNascita().c_str());
            pElement->InsertEndChild(pLN);

            XMLElement * pDN = xmlDoc.NewElement("dataNascita");
            pDN->SetText(candidatiLista.at(indexCandidati).getDataNascita().c_str());
            pElement->InsertEndChild(pDN);


        }
    }



    //store scheda to db
    XMLPrinter printer;
    xmlDoc.Print( &printer );
    string schedaStr = printer.CStr();
    cout << schedaStr << endl;

    PreparedStatement *pstmt;
    pstmt = connection->prepareStatement("INSERT INTO SchedeVoto (fileScheda,idProceduraVoto) VALUES(?,?)");
    try{
        std::stringstream ss(schedaStr);
        pstmt->setBlob(1,&ss);
        pstmt->setUInt(2,scheda->getIdProceduraVoto());
        pstmt->executeUpdate();
        connection->commit();
        cout << "Scheda inserita per la procedura: " << scheda->getIdProceduraVoto() << endl;
    }catch(SQLException &ex){
        cout << "Exception occurred: " << ex.getErrorCode() <<endl;
    }


    //"+to_string(idScheda)+"
    string nomeFile = "schedaVoto.xml";
    XMLError eResult = xmlDoc.SaveFile(nomeFile.c_str());

    //verifichiamo il numero di schede inserite per la procedura per cui si è inserita la scheda
    unsigned int numSchedeInserite = 0;
    ResultSet * resultSet;
    pstmt = connection->prepareStatement("SELECT * FROM SchedeVoto WHERE idProceduraVoto=?");
    try{
        pstmt->setUInt(1,scheda->getIdProceduraVoto());
        resultSet = pstmt->executeQuery();

        while(resultSet->next()){
            numSchedeInserite++;
        }
        cout << "Per la procedura: " << scheda->getIdProceduraVoto() << " le schede inserite sono: " << numSchedeInserite << endl;
    }catch(SQLException &ex){
        cout << "Exception occurred: " << ex.getErrorCode() <<endl;
    }
    delete resultSet;
    //verifichiamo il numero di schede richieste per la procedura per cui si è inserita la scheda

    uint numSchedeRichieste = 0;
    pstmt = connection->prepareStatement("SELECT numSchede FROM ProcedureVoto WHERE idProceduraVoto=?");
    try{
        pstmt->setUInt(1,scheda->getIdProceduraVoto());
        resultSet = pstmt->executeQuery();
        if(resultSet->next()){
            numSchedeRichieste = resultSet->getUInt("numSchede");
        }
        cout << "Le schede richieste sono: " << numSchedeRichieste  << endl;
    }catch(SQLException &ex){
        cout << "Exception occurred: " << ex.getErrorCode() <<endl;
    }


    //se il numero di schede è quello richiesto, aggiorniamo lo stato della procedura su: programmata
    if(numSchedeInserite==numSchedeRichieste){
        pstmt = connection->prepareStatement("UPDATE `ProcedureVoto` SET `stato`='programmata' schedeInserite=? WHERE `idProceduraVoto`=?");
        try{
            pstmt->setUInt(1,numSchedeInserite);
            pstmt->setUInt(2,scheda->getIdProceduraVoto());
            pstmt->executeUpdate();
            connection->commit();
        }catch(SQLException &ex){
            cout << "Exception occurred: " << ex.getErrorCode() <<endl;
        }
        cout << "tutte le schede sono state inserite per la procedura: " << scheda->getIdProceduraVoto()  << endl;
    }
    else{
        pstmt = connection->prepareStatement("UPDATE `ProcedureVoto` SET schedeInserite=? WHERE `idProceduraVoto`=?");
        try{
            pstmt->setUInt(1,numSchedeInserite);
            pstmt->setUInt(2,scheda->getIdProceduraVoto());
            pstmt->executeUpdate();
            connection->commit();
        }catch(SQLException &ex){
            cout << "Exception occurred: " << ex.getErrorCode() <<endl;
        }
    }


    pstmt->close();
    delete pstmt;




    if (eResult != XML_SUCCESS) {
        printf("XMLError: %i\n", eResult);
    }
    else{
        emit storedSchedaVoto();
    }

}

void DataManager::storeRP(ResponsabileProcedimento *rp)
{
    string userid;
    string vergin_userid = rp->getNome() + "." + rp->getCognome();
    remove_copy( vergin_userid.begin() , vergin_userid.end() , back_inserter( userid ) , ' ');
    transform(userid.begin(), userid.end(), userid.begin(), ::tolower);
    //verifichiamo che l'userid sia già presente, in caso positivo va aggiunto un digit random alla fine e si riprova
    bool useridJustExist = true; //supponiamo inizialmente che esista
    ResultSet *resultSet=NULL;
    PreparedStatement *pstmt;
    pstmt = connection->prepareStatement("SELECT * FROM ResponsabiliProcedimento WHERE userid = ?");

    try{

        while(useridJustExist){
            useridJustExist = false;
            pstmt->setString(1,userid);
            resultSet=pstmt->executeQuery();

            //resultSet avrà al più un elemento perché userid è chiave primaria nel db
            while(resultSet->next()){
                //userid già presente
                useridJustExist = true;
                cout << "L'userid: " << userid << " è già presente." << endl;

                int seed = QDateTime::currentDateTime().time().second();
                cout << "Seed: " << seed << endl;
                srand(seed);
                int digitToAdd = rand() % 10;

                userid = userid + ::to_string(digitToAdd);
                cout << "Riprovo con l'userid: " << userid << endl;
            }
        }
    }catch(SQLException &ex){
        cout << "Exception occurred: " << ex.getErrorCode() <<endl;
    }
    delete resultSet;

    //genera coppia di chiavi asimmetriche
    AutoSeededRandomPool rnd;

    RSA::PrivateKey rsaPrivate;
    rsaPrivate.GenerateRandomWithKeySize(rnd, 3072);

    RSA::PublicKey rsaPublic(rsaPrivate);

    ByteQueue queue;
    rsaPublic.Save(queue);
    HexEncoder encoder;
    queue.CopyTo(encoder);
    encoder.MessageEnd();
    string s;
    StringSink ss(s);
    encoder.CopyTo(ss);
    ss.MessageEnd();
    cout << "publicKey: " << s << endl;
    std::stringstream rsaPublicBlob(s);

    //la privateKey è salvata sul DB per comodità temporanea di sviluppo, ma non è la cosa corretta da fare
    ByteQueue queue2;
    rsaPrivate.Save(queue2);
    HexEncoder encoder2;
    queue2.CopyTo(encoder2);
    encoder2.MessageEnd();
    string s2;
    StringSink ss2(s2);
    encoder.CopyTo(ss2);
    ss2.MessageEnd();
    cout << "privateKey: " << s2 << endl;
    std::stringstream rsaPrivateBlob(s2);

    pstmt=connection->prepareStatement
            ("INSERT INTO ResponsabiliProcedimento ( userid, nome, cognome, dataNascita, luogoNascita,publicKey,privateKey ) VALUES (?,?,?,?,?,?,?)");
    try{
        cout << "Memorizzazione del nuovo RP in corso..." << endl;
        pstmt->setString(1,userid);
        pstmt->setString(2,rp->getNome());
        pstmt->setString(3,rp->getCognome());
        pstmt->setDateTime(4,rp->getDataNascita());
        pstmt->setString(5,rp->getLuogoNascita());
        pstmt->setBlob(6,&rsaPublicBlob);
        pstmt->setBlob(7,&rsaPrivateBlob);
        pstmt->executeUpdate();
        connection->commit();
    }catch(SQLException &ex){
        cout<<"Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;


    cout << "UserId del nuovo RP: " << userid << endl;
    QString qsUserid = QString::fromStdString(userid);


    //memorizzazione sul db della coppia userid-password
    storePassNewRP(userid,rp->getPassword());
    //emetto il segnale che l'RP è stato memorizzato e gli passo l'userid di RP come parametro
    emit storedRP(qsUserid);
}

void DataManager::getRPSFromDB()
{
    vector <ResponsabileProcedimento> listRPS;
    ResultSet *resultSet=NULL;
    PreparedStatement *pstmt;
    pstmt = connection->prepareStatement("SELECT * FROM ResponsabiliProcedimento");
    try{
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            ResponsabileProcedimento rp;
            rp.setNome(resultSet->getString("nome"));
            rp.setCognome(resultSet->getString("cognome"));
            QString qsDataNascita = QString::fromStdString(resultSet->getString("dataNascita"));
            QDate dataNascita = QDate::fromString(qsDataNascita,"yyyy-MM-dd");
            rp.setDataNascita(dataNascita.toString("dd/MM/yyyy").toStdString());
            rp.setLuogoNascita(resultSet->getString("luogoNascita"));
            rp.setUserid(resultSet->getString("userid"));
            rp.setIdRP(resultSet->getInt("idResponsabileProcedimento"));
            listRPS.push_back(rp);
        }
    }catch(SQLException &ex){
        cout<<"Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
    delete resultSet;

    emit readyRPS(listRPS);
}

void DataManager::getProcedureVotoFromDB()
{
    QList <ProceduraVoto> listPVs;

    PreparedStatement *pstmt;
    ResultSet *resultSet;
    pstmt = connection->prepareStatement("SELECT * FROM ProcedureVoto");

    try{
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            ProceduraVoto pv;
            pv.setIdProceduraVoto(resultSet->getUInt("idProceduraVoto"));
            pv.setIdRP(resultSet->getUInt("idResponsabileProcedimento"));
            pv.setDescrizione(resultSet->getString("descrizione"));

            QString qsInizio = QString::fromStdString(resultSet->getString("inizio")); //format is: yyyy-MM-dd hh:mm
            QDateTime dt1 = QDateTime::fromString(qsInizio,"yyyy-MM-dd hh:mm:ss");
            pv.setData_ora_inizio(dt1.toString("dd/MM/yyyy hh:mm").toStdString());

            QString qsFine = QString::fromStdString(resultSet->getString("fine"));
            QDateTime dt2 = QDateTime::fromString(qsFine,"yyyy-MM-dd hh:mm:ss");
            pv.setData_ora_termine(dt2.toString("dd/MM/yyyy hh:mm").toStdString());

            pv.setNumSchedeVoto(resultSet->getUInt("numSchede"));
            pv.setStato(resultSet->getString("stato"));
            pv.setSchedeInserite(resultSet->getUInt("schedeInserite"));
            listPVs.append(pv);
        }

    }catch(SQLException &ex){
        cout<<"Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
    delete resultSet;

    //TODO ottenere procedure memorizzate nel DB

    emit readyProcedure(listPVs);
}

void DataManager::deleteProceduraVoto(uint idProceduraVoto)
{
    PreparedStatement * pstmt;
    pstmt = connection->prepareStatement("DELETE FROM `ProcedureVoto` WHERE `idProceduraVoto`=?");
    try{
        pstmt->setUInt(1,idProceduraVoto);
        pstmt->executeQuery();
        connection->commit();
    }catch(SQLException &ex){
        cout<<"Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;

    emit deletedProcedura();

}

void DataManager::storePassNewRP(string userid, string pass)
{
    /*Generate salt */
    AutoSeededRandomPool prng;
    SecByteBlock salt(AES::BLOCKSIZE);
    prng.GenerateBlock(salt, sizeof(salt));
    string s;
    HexEncoder hex(new StringSink(s));
    hex.Put(salt, salt.size());
    hex.MessageEnd();


    string hashedPass = hashPassword(pass, s);
    cout << "salt:" << s << ", size: " << s.size() << endl;
    cout << "Hash della Password: " << hashedPass  << ", size: " << hashedPass.size()<< endl;

    PreparedStatement *pstmt;

    pstmt = connection->prepareStatement("INSERT INTO Utenti (userid,salt,hashedPassword) VALUES(?,?,?)");
    try{
        pstmt->setString(1,userid);
        pstmt->setString(2,s);
        pstmt->setString(3,hashedPass);
        pstmt->executeUpdate();
        connection->commit();
    }catch(SQLException &ex){
        cout<<"Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
}

void DataManager::storeProcedura(ProceduraVoto *procedura)
{
    // memorizzare procedura nel db
    PreparedStatement *pstmt;


    pstmt = connection->prepareStatement
            ("INSERT INTO ProcedureVoto (idResponsabileProcedimento, descrizione, numSchede, stato, ultimaModifica, inizio, fine) VALUES(?,?,?,?,?,?,?)");
    string currentTime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss").toStdString();
    try{
        pstmt->setUInt(1,procedura->getIdRP());
        pstmt->setString(2,procedura->getDescrizione());
        pstmt->setUInt(3,procedura->getNumSchedeVoto());
        pstmt->setString(4,"creazione");
        pstmt->setDateTime(5,currentTime);
        pstmt->setDateTime(6,procedura->getData_ora_inizio());
        pstmt->setDateTime(7,procedura->getData_ora_termine());
        pstmt->executeUpdate();
        connection->commit();
    }catch(SQLException &ex){
        cout<<"Exception occurred: "<<ex.getErrorCode()<<endl;
    }

    cout << "after insert procedura" << endl;

    pstmt = connection->prepareStatement("SELECT LAST_INSERT_ID() AS idProceduraVoto");
    ResultSet *resultSet=NULL;
    uint idProceduraInserita;
    try{
        resultSet = pstmt->executeQuery();
        resultSet->next();
        idProceduraInserita = resultSet->getUInt("idProceduraVoto");

    }catch(SQLException &ex){
        cout<<"Exception occurred: "<< ex.getErrorCode()<<endl;
    }

    cout << "La procedura appena inserita ha id: " << idProceduraInserita << endl;

    //memorizzazione sessioni
    vector <SessioneVoto> sessioni = procedura->getSessioni();
    pstmt = connection->prepareStatement("INSERT INTO Sessioni (idProceduraVoto,data,apertura,chiusura) VALUES(?,?,?,?)");
    try{
        for(unsigned int i = 0; i < sessioni.size(); i++){
            pstmt->setUInt(1,idProceduraInserita);
            pstmt->setDateTime(2,sessioni.at(i).getData());
            pstmt->setString(3,sessioni.at(i).getOraApertura());
            pstmt->setString(4,sessioni.at(i).getOraChiusura());
            pstmt->executeUpdate();
            connection->commit();
        }
    }catch(SQLException &ex){
        cout<<"Exception occurred: "<< ex.getErrorCode()<<endl;
    }

    pstmt->close();
    delete pstmt;

    emit storedProcedura();
}

string DataManager::hashPassword( string plainPass, string salt){
    SecByteBlock result(32);
    string hexResult;

    PKCS5_PBKDF2_HMAC<SHA256> pbkdf;

    pbkdf.DeriveKey(result, result.size(),0x00,(byte *) plainPass.data(), plainPass.size(),(byte *) salt.data(), salt.size(),100);

    //ArraySource resultEncoder(result,result.size(), true, new HexEncoder(new StringSink(hexResult)));

    HexEncoder hex(new StringSink(hexResult));
    hex.Put(result.data(), result.size());
    hex.MessageEnd();

    return hexResult;

}


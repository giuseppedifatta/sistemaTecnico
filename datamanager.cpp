#include "datamanager.h"
//#include "mysql_driver.h"

//#ifndef XMLCheckResult
//#define XMLCheckResult(a_eResult)
//#endif

#include <istream>
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

void DataManager::storeProcedura(ProceduraVoto *procedura)
{
    //TODO memorizzare procedura nel db
}

void DataManager::getRPSFromDB()
{
    //TODO ottenere dal DB informazioni dei responsabili di procedimento registrati nel sistema
    vector <ResponsabileProcedimento> listRPS;
    ResultSet *resultSet=NULL;
    PreparedStatement *pstmt;
    pstmt = connection->prepareStatement("SELECT * FROM ResponsabiliProcedimento");
    try{
        resultSet=pstmt->executeQuery();
        while(resultSet->next()){
            ResponsabileProcedimento rp;
            rp.setNome(resultSet->getString("nome"));
            rp.setCognome(resultSet->getString("cognome"));
            rp.setDataNascita(resultSet->getString("dataNascita"));
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


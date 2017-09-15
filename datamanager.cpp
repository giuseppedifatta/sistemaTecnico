#include "datamanager.h"
//#include "mysql_driver.h"

//#ifndef XMLCheckResult
//#define XMLCheckResult(a_eResult)
//#endif

#include <istream>
#include <QDebug>

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    try{
        driver=get_driver_instance();
        connection=driver->connect("localhost:3306","root", "root");
        connection->setAutoCommit(false);
        connection->setSchema("mydb");
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }

    //decommentare per impostare le password seguenti come password di tecnico e superuser
    //    string tecnicoPass;
    //    string suPass;
    //    tecnicoPass= "tecnico";
    //    suPass = "admin";
    //    storePassNewUser("tecnico", tecnicoPass);
    //    storePassNewUser("superuser", suPass);

    cout << "MySql Server ok." << endl;
}

DataManager::~DataManager()
{
    delete connection;
    //delete driver;
}

void DataManager::checkPassTecnico(QString passTecnico)
{
    if(verifyUserPass("tecnico",passTecnico.toStdString()))
    {
        cout << "Granted access to tecnico" << endl;
        emit passOK();
    }
    else{
        emit wrongTecnicoPass();
    }
}
bool DataManager::verifyUserPass(string userid,string userPass){
    //ottengo dal database salt e hash della password del tecnico
    string storedSalt;
    string storedHashedPassword;

    PreparedStatement * pstmt;
    ResultSet * resultSet;
    pstmt = connection->prepareStatement("SELECT salt, hashedPassword FROM Utenti WHERE userid = ?");
    try{
        pstmt->setString(1,userid);
        resultSet = pstmt->executeQuery();
        if(resultSet->next()){
            storedSalt = resultSet->getString("salt");
            storedHashedPassword = resultSet->getString("hashedPassword");
        }
    }catch(SQLException &ex){
        cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
    }
    pstmt->close();
    delete pstmt;
    delete resultSet;

    string calculatedHashedPassword = hashPassword(userPass,storedSalt);

    if(calculatedHashedPassword==storedHashedPassword){
        return true;
    }
    else {
        return false;
    }
}
void DataManager::tryChangeTecnicoPass(QString su_pass, QString newTecnicoPass)
{
    if(verifyUserPass("superuser",su_pass.toStdString())){

        updateUserPassword("tecnico",newTecnicoPass.toStdString());

        emit tecnicoPassChanged();
    }
    else{
        emit wrongSUpass();
    }
}

void DataManager::updateUserPassword(string userid,string nuovaPassword){
    /*Generate salt */
    AutoSeededRandomPool prng;
    SecByteBlock salt(AES::BLOCKSIZE);
    prng.GenerateBlock(salt, sizeof(salt));
    string strSalt;
    HexEncoder hex(new StringSink(strSalt));
    hex.Put(salt, salt.size());
    hex.MessageEnd();


    string hashedPass = hashPassword(nuovaPassword, strSalt);
    cout << "salt:" << strSalt << ", size: " << strSalt.size() << endl;
    cout << "Hash della Password: " << hashedPass  << ", size: " << hashedPass.size()<< endl;

    PreparedStatement *pstmt;

    pstmt = connection->prepareStatement("UPDATE Utenti SET salt = ? , hashedPassword = ? WHERE userid = ?");
    try{
        pstmt->setString(1,strSalt);
        pstmt->setString(2,hashedPass);
        pstmt->setString(3,userid);
        pstmt->executeUpdate();
        connection->commit();
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
}

void DataManager::storeScheda(SchedaVoto *scheda)
{
    // estrazione dati dall'oggetto di tipo scheda voto, e generazione del file XML, con conseguente memorizzazione su database

    XMLDocument xmlDoc;
    XMLNode * pRoot = xmlDoc.NewElement("SchedaVoto");
    xmlDoc.InsertFirstChild(pRoot);
    XMLElement * pElement;

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
        cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
    }

    //aggiungiamo l'id univoco della scheda al file xml appena memorizzato nel database
    pstmt = connection->prepareStatement("SELECT LAST_INSERT_ID() AS codSchedaVoto");
    ResultSet *resultSet;
    uint codSchedaVoto;
    try{
        resultSet = pstmt->executeQuery();
        resultSet->next();
        codSchedaVoto = resultSet->getUInt("codSchedaVoto");

    }catch(SQLException &ex){
        cerr << "Exception occurred: "<< ex.getErrorCode()<<endl;
    }
    pElement = xmlDoc.NewElement("id");
    pElement->SetText(codSchedaVoto);
    pRoot->InsertEndChild(pElement);

    //---

    //update file scheda con l'id univoco identificativo come suo campo
    //XMLPrinter printer;
    printer.ClearBuffer();
    xmlDoc.Print( &printer );
    schedaStr = printer.CStr();
    cout << schedaStr << endl;

    //PreparedStatement *pstmt;
    pstmt = connection->prepareStatement("UPDATE SchedeVoto SET fileScheda=? WHERE codSchedaVoto=? ");
    try{
        std::stringstream ss(schedaStr);
        pstmt->setBlob(1,&ss);
        pstmt->setUInt(2,codSchedaVoto);
        pstmt->executeUpdate();
        connection->commit();
        cout << "Aggiunto id alla scheda: " << codSchedaVoto << endl;
    }catch(SQLException &ex){
        cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
    }



    //"+to_string(idScheda)+"
    string nomeFile = "schedaVoto.xml";
    XMLError eResult = xmlDoc.SaveFile(nomeFile.c_str());

    //verifichiamo il numero di schede inserite per la procedura per cui si è inserita la scheda
    uint numSchedeInserite = 0;
    //ResultSet * resultSet;
    pstmt = connection->prepareStatement("SELECT * FROM SchedeVoto WHERE idProceduraVoto=?");
    try{
        pstmt->setUInt(1,scheda->getIdProceduraVoto());
        resultSet = pstmt->executeQuery();

        while(resultSet->next()){
            numSchedeInserite++;
        }
        cout << "Per la procedura: " << scheda->getIdProceduraVoto() << " le schede inserite sono: " << numSchedeInserite << endl;
    }catch(SQLException &ex){
        cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
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
        cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
    }
    pstmt->close();
    delete pstmt;

    //se il numero di schede è quello richiesto, aggiorniamo lo stato della procedura su: programmata
    if(numSchedeInserite==numSchedeRichieste){
        cout << "Aggiornamento numero schede inserite e stato della procedura su <programmata>" << endl;
        PreparedStatement * pstmt;
        pstmt = connection->prepareStatement("UPDATE `ProcedureVoto` SET stato=?, schedeInserite=? WHERE idProceduraVoto=?");
        try{
            pstmt->setUInt(1,ProceduraVoto::statiProcedura::programmata);
            pstmt->setUInt(2,numSchedeInserite);
            pstmt->setUInt(3,scheda->getIdProceduraVoto());
            pstmt->executeUpdate();
            connection->commit();
        }catch(SQLException &ex){
            cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
        }
        pstmt->close();
        delete pstmt;
        cout << "tutte le schede sono state inserite per la procedura: " << scheda->getIdProceduraVoto()  << endl;
    }
    else{
        cout << "Aggiornamento del numero di schede inserite" << endl;
        PreparedStatement * pstmt;
        pstmt = connection->prepareStatement("UPDATE `ProcedureVoto` SET schedeInserite=? WHERE `idProceduraVoto`=?");
        try{

            pstmt->setUInt(1,numSchedeInserite);
            pstmt->setUInt(2,scheda->getIdProceduraVoto());
            pstmt->executeUpdate();
            connection->commit();
        }catch(SQLException &ex){
            cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
        }
        pstmt->close();
        delete pstmt;
    }





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
        cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
    }
    delete resultSet;

    //genera coppia di chiavi asimmetriche
    AutoSeededRandomPool rnd;

    RSA::PrivateKey rsaPrivate;
    rsaPrivate.GenerateRandomWithKeySize(rnd, 3072);

    RSA::PublicKey rsaPublic(rsaPrivate);

    //conversione della chiave pubblica in stringa esadecimale
    ByteQueue queue;
    rsaPublic.Save(queue);

    string publicKey;
    StringSink ss1(publicKey);
    queue.CopyTo(ss1);
    ss1.MessageEnd();
    cout << "publicKey: " << publicKey << endl;

    //encoding esadecimale della publicKey
    string publicKeyEncoded;
    StringSource (publicKey, true /*pump all*/,
                  new HexEncoder(
                      new StringSink(publicKeyEncoded)
                      ) // HexEncoder
                  ); // StringSource
    cout << "publicKey encoded: " << publicKeyEncoded << endl;

    //chiave pubblica pronta per essere memorizzata sul DB
    std::stringstream rsaPublicBlob(publicKeyEncoded);

    //calcoliamo dalla password scelta dall'RP una derivedKey per cifrare la privateKey di RP
    string pass = rp->getPassword();
    string derivedKeyEncoded = deriveKeyFromPass(pass);
    cout << "derivedKey ottenuta dalla password di RP: " << derivedKeyEncoded << endl;


    //copio la chiave privata in una stringa
    ByteQueue queue2;
    rsaPublic.Save(queue2);

    string privateKey;
    StringSink ss2(privateKey);
    queue.CopyTo(ss2);
    ss2.MessageEnd();
    cout << "privateKey: " << privateKey << endl;

    //---cifratura della chiave privata----
    //decodifica della chiave derivata dalla password di RP
    string derivedKeyDecoded;
    StringSource(derivedKeyEncoded,true,
                 new HexDecoder(
                     new StringSink(derivedKeyDecoded)
                     ) // HexDecoder
                 ); // StringSource

    SecByteBlock key(reinterpret_cast<const byte*>(derivedKeyDecoded.data()), derivedKeyDecoded.size());


    //Questo IV deve essere lo stesso in fase di decifratura
    byte iv[AES::MAX_KEYLENGTH];
    memset(iv, 0x01,AES::BLOCKSIZE);

    //cifriamo la chiave privata di RP con chiave simmetrica
    string privateKeyCifrata = encryptStdString(privateKey,key,iv);

    //rendiamo la chiave cifrata in esadecimale
    string encodedChiaveRPCifrata;
    StringSource (privateKeyCifrata, true,
                  new HexEncoder(
                      new StringSink(encodedChiaveRPCifrata)
                      ) // HexEncoder
                  ); // StringSourceEnd();

    cout << "privateKey cifrata encoded: " << encodedChiaveRPCifrata << endl;
    //chiave privata cifrata pronta per essere memorizzata sul DB
    std::stringstream rsaPrivateBlob(encodedChiaveRPCifrata);

    pstmt=connection->prepareStatement
            ("INSERT INTO ResponsabiliProcedimento ( userid, nome, cognome, dataNascita, luogoNascita,publicKey,encryptedPrivateKey ) VALUES (?,?,?,?,?,?,?)");
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
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;


    cout << "UserId del nuovo RP: " << userid << endl;
    QString qsUserid = QString::fromStdString(userid);


    //memorizzazione sul db della coppia userid-password
    storePassNewUser(userid,rp->getPassword());
    //emetto il segnale che l'RP è stato memorizzato e gli passo l'userid di RP come parametro
    emit storedRP(qsUserid);
}
string DataManager::encryptStdString(string plain, SecByteBlock key, byte* iv) {


    string cipher, encoded;

    /*********************************\
    \*********************************/

    // Pretty print key
    encoded.clear();
    StringSource(key, key.size(), true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "key: " << encoded << endl;

    // Pretty print iv
    encoded.clear();
    std::string s_iv( reinterpret_cast< char const* >(iv) ) ;
    StringSource(s_iv, true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "iv: " << encoded << endl;

    /*********************************\
    \*********************************/

    try
    {
        cout << "plain text: " << plain << endl;

        CBC_Mode< AES >::Encryption aesEncryptor;
        aesEncryptor.SetKeyWithIV(key, key.size(), iv);

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource (plain, true,
                      new StreamTransformationFilter(aesEncryptor,
                                                     new StringSink(cipher)
                                                     ) // StreamTransformationFilter
                      ); // StringSource


    }
    catch(const CryptoPP::Exception& e)
    {
        cerr << "Caught exception :" << e.what() << endl;
    }

    /*********************************\
    \*********************************/

    // Pretty print
    encoded.clear();
    StringSource(cipher, true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "cipher text: " << encoded << endl;

    return cipher;
}

string DataManager::encryptStdString(string plain, SecByteBlock key, SecByteBlock  iv) {


    string cipher, encoded;

    /*********************************\
    \*********************************/

    // Pretty print key
    encoded.clear();
    StringSource(key, key.size(), true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "key: " << encoded << endl;

    // Pretty print iv
    encoded.clear();
    StringSource(iv,iv.size(),true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "iv: " << encoded << endl;

    /*********************************\
    \*********************************/

    try
    {
        cout << "plain text: " << plain << endl;

        CBC_Mode< AES >::Encryption aesEncryptor;
        aesEncryptor.SetKeyWithIV(key, key.size(), iv);

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource (plain, true,
                      new StreamTransformationFilter(aesEncryptor,
                                                     new StringSink(cipher)
                                                     ) // StreamTransformationFilter
                      ); // StringSource


    }
    catch(const CryptoPP::Exception& e)
    {
        cerr << "Caught exception :" << e.what() << endl;
    }

    /*********************************\
    \*********************************/

    // Pretty print
    encoded.clear();
    StringSource(cipher, true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "cipher text: " << encoded << endl;

    return cipher;
}

string DataManager::decryptStdString(string cipher, SecByteBlock key, byte* iv){
    string encoded,recovered;
    encoded.clear();
    StringSource(key, key.size(), true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "key: " << encoded << endl;

    // Pretty print iv
    encoded.clear();
    std::string s_iv( reinterpret_cast< char const* >(iv) ) ;
    StringSource(s_iv, true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "iv: " << encoded << endl;
    try
    {

        CBC_Mode< AES >::Decryption aesDecryptor;
        aesDecryptor.SetKeyWithIV(key, key.size(), iv);

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource (cipher, true,
                      new StreamTransformationFilter(aesDecryptor,
                                                     new StringSink(recovered)
                                                     ) // StreamTransformationFilter
                      ); // StringSource

        cout << "recovered text: " << recovered << endl;
    }
    catch(const CryptoPP::Exception& e)
    {
        cerr << "Caught exception :" << e.what() << endl;
    }
    return recovered;
}

string DataManager::decryptStdString(string cipher, SecByteBlock key, SecByteBlock iv){
    string encoded,recovered;
    encoded.clear();
    StringSource(key, key.size(), true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "key: " << encoded << endl;

    // Pretty print iv
    encoded.clear();
    StringSource(iv,iv.size(),true,
                 new HexEncoder(
                     new StringSink(encoded)
                     ) // HexEncoder
                 ); // StringSource
    cout << "iv: " << encoded << endl;
    try
    {

        CBC_Mode< AES >::Decryption aesDecryptor;
        aesDecryptor.SetKeyWithIV(key, key.size(), iv);

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource (cipher, true,
                      new StreamTransformationFilter(aesDecryptor,
                                                     new StringSink(recovered)
                                                     ) // StreamTransformationFilter
                      ); // StringSource

        cout << "recovered text: " << recovered << endl;
    }
    catch(const CryptoPP::Exception& e)
    {
        cerr << "Caught exception :" << e.what() << endl;
    }
    return recovered;
}


string DataManager::deriveKeyFromPass(string password){
    string derivedKey;
    try {

        // KDF parameters
        unsigned int iterations = 15000;
        char purpose = 0; // unused by Crypto++

        // 32 bytes of derived material. Used to key the cipher.
        //   16 bytes are for the key, and 16 bytes are for the iv.
        SecByteBlock derived(AES::MAX_KEYLENGTH);

        // KDF function
        PKCS5_PBKDF2_HMAC<SHA256> kdf;
        kdf.DeriveKey(derived.data(), derived.size(), purpose, (byte*)password.data(), password.size(), NULL, 0, iterations);




        // Encode derived
        HexEncoder hex(new StringSink(derivedKey));
        hex.Put(derived.data(), derived.size());
        hex.MessageEnd();

        // Print stuff
        cout << "pass: " << password << endl;
        //cout << "derived key: " << derivedKey << endl;

    }
    catch(CryptoPP::Exception& ex)
    {
        cerr << ex.what() << endl;
    }
    return derivedKey;
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
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
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
            uint idProceduraVoto = resultSet->getUInt("idProceduraVoto");
            pv.setIdProceduraVoto(idProceduraVoto);
            pv.setIdRP(resultSet->getUInt("idResponsabileProcedimento"));
            pv.setDescrizione(resultSet->getString("descrizione"));

            QString qsInizio = QString::fromStdString(resultSet->getString("inizio")); //format is: yyyy-MM-dd hh:mm
            QDateTime dtInizio = QDateTime::fromString(qsInizio,"yyyy-MM-dd hh:mm:ss");
            pv.setData_ora_inizio(dtInizio.toString("dd/MM/yyyy hh:mm").toStdString());

            QString qsFine = QString::fromStdString(resultSet->getString("fine"));
            QDateTime dtFine = QDateTime::fromString(qsFine,"yyyy-MM-dd hh:mm:ss");
            pv.setData_ora_termine(dtFine.toString("dd/MM/yyyy hh:mm").toStdString());

            uint numSchedeVoto = resultSet->getUInt("numSchede");
            pv.setNumSchedeVoto(numSchedeVoto);
            uint schedeInserite = resultSet->getUInt("schedeInserite");
            pv.setSchedeInserite(schedeInserite);


            ProceduraVoto::statiProcedura statoOttenuto = (ProceduraVoto::statiProcedura)resultSet->getUInt("stato");
            QDateTime dtCorrente = QDateTime::currentDateTime();
            ProceduraVoto::statiProcedura statoProceduraAggiornato = statoOttenuto;
            uint statoVotantiResettato;
            bool correzione = false;
            bool resetStatoVotanti = false;
            if((dtCorrente >= dtInizio && dtCorrente <= dtFine)&&(statoOttenuto!=ProceduraVoto::statiProcedura::in_corso)){
                //correzione necessaria, verifichiamo quale
                if(numSchedeVoto==schedeInserite){
                    correzione = true;
                    statoProceduraAggiornato = ProceduraVoto::statiProcedura::in_corso;
                    //resettiamo lo stato di votanti per la procedura che sta per iniziare
                    statoVotantiResettato = statoVoto::non_espresso;
                    resetStatoVotanti = true;
                }
                else{
                    correzione = true;
                    statoProceduraAggiornato = ProceduraVoto::statiProcedura::da_eliminare;
                }
            }
            else if(dtCorrente > dtFine){
                if(numSchedeVoto==schedeInserite){
                    if((statoOttenuto!=ProceduraVoto::statiProcedura::conclusa)&&(statoOttenuto!=ProceduraVoto::statiProcedura::scrutinata)){
                        correzione = true;
                        statoProceduraAggiornato = ProceduraVoto::statiProcedura::conclusa;
                    }
                }
                else{
                    correzione = true;
                    statoProceduraAggiornato = ProceduraVoto::statiProcedura::da_eliminare;
                }
            }

            //setta nella procedura da restituire alla view lo stato eventualmente corretto
            //a seguire, l'eventuale aggiornamento sul model
            pv.setStato(statoProceduraAggiornato);

            listPVs.append(pv);

            //aggiornamento sul model dello stato della procedura corretto
            if(correzione){
                cout << "Correzione stato procedura: " << statoProceduraAggiornato << endl;
                PreparedStatement *pstmt2;

                pstmt2 = connection->prepareStatement("UPDATE ProcedureVoto SET stato=? WHERE idProceduraVoto=?");
                try{
                    pstmt2->setUInt(1,statoProceduraAggiornato); //in_corso
                    pstmt2->setUInt(2,idProceduraVoto);

                    pstmt2->executeUpdate();
                    connection->commit();
                }catch(SQLException &ex){
                    cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
                }
                pstmt2->close();
                delete pstmt2;

            }
            if(resetStatoVotanti){
                cout << "Una nuova procedura è in corso. Reset stato votanti: " << statoVotantiResettato << endl;
                PreparedStatement *pstmt2;

                pstmt2 = connection->prepareStatement("UPDATE Anagrafica SET statoVoto = ?");
                try{
                    pstmt2->setUInt(1,statoVotantiResettato); //non_espresso
                    pstmt2->executeUpdate();
                    connection->commit();
                }catch(SQLException &ex){
                    cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
                }
                pstmt2->close();
                delete pstmt2;
            }
        }

    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
    delete resultSet;

    emit readyProcedure(listPVs);
}

void DataManager::getSessioniProceduraFromDB(uint idProcedura)
{
    QList <SessioneVoto> sessioni;

    PreparedStatement * pstmt;
    ResultSet * resultSet;
    pstmt = connection->prepareStatement("SELECT * FROM Sessioni WHERE idProceduraVoto = ?");
    try{
        pstmt->setUInt(1,idProcedura);
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            SessioneVoto s;

            QString qsData = QString::fromStdString(resultSet->getString("data")); //format is: yyyy-MM-dd
            QDate data = QDate::fromString(qsData,"yyyy-MM-dd");
            s.setData(data.toString("dd/MM/yyyy").toStdString());

            QString qsApertura = QString::fromStdString(resultSet->getString("apertura")); //format is: yyyy-MM-dd
            QTime tApertura = QTime::fromString(qsApertura,"hh:mm:ss");
            s.setOraApertura(tApertura.toString("hh:mm").toStdString());

            QString qsChiusura = QString::fromStdString(resultSet->getString("chiusura")); //format is: yyyy-MM-dd
            QTime tChiusura = QTime::fromString(qsChiusura,"hh:mm:ss");
            s.setOraChiusura(tChiusura.toString("hh:mm").toStdString());

            s.setIdSessione(resultSet->getUInt("idSessione"));
            sessioni.append(s);
        }
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
    delete resultSet;

    emit readySessioni(sessioni);
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
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;

    emit deletedProcedura();

}

void DataManager::getSchedeProceduraFromDB(uint idProcedura)
{
    QList <SchedaVoto> schede;

    PreparedStatement *pstmt;
    ResultSet *resultSet;
    pstmt = connection->prepareStatement("SELECT * FROM SchedeVoto WHERE idProceduraVoto = ?");
    try{
        pstmt->setUInt(1,idProcedura);
        resultSet = pstmt->executeQuery();

        //per ogni scheda ottengo il contenuto e aggiungo alla lista delle schede
        while(resultSet->next()){
            SchedaVoto s;
            unsigned int idScheda = resultSet->getUInt("codSchedaVoto");
            s.setId(idScheda);

            std::istream *blobData = resultSet->getBlob("fileScheda");
            std::istreambuf_iterator<char> isb = std::istreambuf_iterator<char>(*blobData);
            std::string blobString = std::string(isb, std::istreambuf_iterator<char>());
            cout << "Scheda: " << idScheda << endl;
            cout << blobString << endl;
            XMLDocument xmlDoc;
            xmlDoc.Parse(blobString.c_str());

            XMLNode *rootNode = xmlDoc.FirstChild();
            XMLText* textNodeIdProcedura = rootNode->FirstChildElement("idProcedura")->FirstChild()->ToText();
            const char * idProceduraArray = textNodeIdProcedura->Value();

            int idProcedura = atoi(idProceduraArray);
            cout << "idProcedura: " << idProcedura << endl;
            s.setIdProceduraVoto(idProcedura);

            XMLText* textNodeTipologiaElezione= rootNode->FirstChildElement("tipologiaElezione")->FirstChild()->ToText();
            uint tipologiaElezione = atoi(textNodeTipologiaElezione->Value());
            cout << "tipologia elezione: " << tipologiaElezione << endl;
            s.setTipoElezione(tipologiaElezione);

            XMLText* textNodeNumeroPreferenze = rootNode->FirstChildElement("numeroPreferenze")->FirstChild()->ToText();
            uint numeroPreferenze = atoi(textNodeNumeroPreferenze->Value());
            cout << "Numero preferenze: " << numeroPreferenze << endl;
            s.setNumPreferenze(numeroPreferenze);

            XMLElement * listeElement = rootNode->FirstChildElement("liste");

            XMLElement * firstListaElement = listeElement->FirstChildElement("lista");
            XMLElement * lastListaElement = listeElement->LastChildElement("lista");

            XMLElement *listaElement = firstListaElement;
            bool lastLista = false;
            do{

                int idLista = listaElement->IntAttribute("id");
                cout << " --- lista trovata" << endl;
                cout << "id Lista: " << idLista << endl;
                string nomeLista = listaElement->Attribute("nome");
                cout << "nome: " << nomeLista << endl;

                XMLElement * firstCandidatoElement  = listaElement->FirstChildElement("candidato");
                XMLElement * lastCandidatoElement = listaElement->LastChildElement("candidato");

                XMLElement * candidatoElement = firstCandidatoElement;
                //ottengo tutti i candidati della lista
                bool lastCandidato = false;
                do{
                    int id = candidatoElement->IntAttribute("id");
                    cout << "trovato candidato, id: " << id << endl;

                    XMLElement * matricolaElement = candidatoElement->FirstChildElement("matricola");
                    XMLNode * matricolaInnerNode = matricolaElement->FirstChild();
                    string matricola;
                    if(matricolaInnerNode!=nullptr){
                        matricola = matricolaInnerNode->ToText()->Value();
                    }
                    cout << matricola << endl;

                    XMLElement *nomeElement = matricolaElement->NextSiblingElement("nome");
                    XMLNode * nomeInnerNode = nomeElement->FirstChild();
                    string nome;
                    if(nomeInnerNode!=nullptr){
                        nome = nomeInnerNode->ToText()->Value();
                    }
                    cout << nome << endl;

                    XMLElement *cognomeElement = nomeElement->NextSiblingElement("cognome");
                    XMLNode * cognomeInnerNode = cognomeElement->FirstChild();
                    string cognome;
                    if(cognomeInnerNode!=nullptr){
                        cognome = cognomeInnerNode->ToText()->Value();
                    }
                    cout << cognome << endl;

                    XMLElement *luogoNascitaElement = cognomeElement->NextSiblingElement("luogoNascita");
                    XMLNode * luogoNascitaInnerNode = luogoNascitaElement->FirstChild();
                    string luogoNascita;
                    if(luogoNascitaInnerNode!=nullptr){
                        luogoNascita = luogoNascitaInnerNode->ToText()->Value();
                    }
                    cout << luogoNascita << endl;

                    XMLElement *dataNascitaElement = luogoNascitaElement->NextSiblingElement("dataNascita");
                    XMLNode * dataNascitaInnerNode = dataNascitaElement->FirstChild();
                    string dataNascita;
                    if(dataNascitaInnerNode!=nullptr){
                        dataNascita = dataNascitaInnerNode->ToText()->Value();
                    }
                    cout << dataNascita << endl;

                    cout << "Estratti i dati del candidato id: " << id << endl;
                    s.addCandidato(matricola,nome,cognome,nomeLista,dataNascita,luogoNascita);

                    //accesso al successivo candidato
                    if(candidatoElement == lastCandidatoElement){
                        lastCandidato = true;
                    }else {
                        candidatoElement = candidatoElement->NextSiblingElement("candidato");
                        cout << "ottengo il puntatore al successivo candidato" << endl;
                    }
                }while(!lastCandidato);

                cout << "non ci sono altri candidati nella lista: " << nomeLista << endl;


                if(listaElement == lastListaElement){
                    lastLista = true;
                }
                else{
                    //accediamo alla successiva lista nella scheda di voto
                    listaElement = listaElement->NextSiblingElement("lista");
                    cout << "ottengo il puntatore alla successiva lista" << endl;
                }
            }while(!lastLista);
            cout << "non ci sono altre liste" << endl;
            schede.append(s);
        }
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
    delete resultSet;

    emit readySchede(schede);
}

void DataManager::checkAvailabilityProceduraRange(QDateTime inizio, QDateTime termine)
{
    bool availability = true;

    QString dbDateTimeInizio = inizio.toString("yyyy/MM/dd hh:mm:ss");
    QString dbDateTimeTermine = termine.toString("yyyy/MM/dd hh:mm:ss");
    PreparedStatement * pstmt;
    ResultSet * resultSet;
    pstmt = connection->prepareStatement("SELECT * FROM ProcedureVoto WHERE "
                                         "(?>=inizio AND ?<=fine) OR "
                                         "(?>=inizio AND ?<=fine) OR "
                                         "(?>=inizio AND ?<=fine) OR "
                                         "(?<=inizio AND ?>=fine)"
                                         );
    try{
        //inizio <=newInizio <= fine
        pstmt->setDateTime(1,dbDateTimeInizio.toStdString());
        pstmt->setDateTime(2,dbDateTimeInizio.toStdString());

        //inizio <=newFine <= fine
        pstmt->setDateTime(3,dbDateTimeTermine.toStdString());
        pstmt->setDateTime(4,dbDateTimeTermine.toStdString());

        //newInizio >= inizio AND newFine <= fine
        pstmt->setDateTime(5,dbDateTimeInizio.toStdString());
        pstmt->setDateTime(6,dbDateTimeTermine.toStdString());

        //newInizio <= inizio AND newFine >= fine
        pstmt->setDateTime(7,dbDateTimeInizio.toStdString());
        pstmt->setDateTime(8,dbDateTimeTermine.toStdString());

        resultSet = pstmt->executeQuery();
        if(resultSet->next()){
            availability = false;
            cout << "Range selezionato sovrapposto ad altre procedure" << endl;
        }
        else{
            cout << "Nuovo range selezionato è disponibile per una nuova procedura: " << dbDateTimeInizio.toStdString() << " - " << dbDateTimeTermine.toStdString() << endl;
        }
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }


    pstmt->close();
    delete pstmt;
    delete resultSet;

    if (availability){
        emit proceduraRangeAvailable(inizio,termine);
    }
    else{
        emit requestedProceduraRangeInUse();
    }
}

void DataManager::storePassNewUser(string userid, string pass)
{
    /*Generate salt */
    AutoSeededRandomPool prng;
    SecByteBlock salt(AES::BLOCKSIZE);
    prng.GenerateBlock(salt, sizeof(salt));
    string strSalt;
    HexEncoder hex(new StringSink(strSalt));
    hex.Put(salt, salt.size());
    hex.MessageEnd();


    string hashedPass = hashPassword(pass, strSalt);
    cout << "salt:" << strSalt << ", size: " << strSalt.size() << endl;
    cout << "Hash della Password: " << hashedPass  << ", size: " << hashedPass.size()<< endl;

    PreparedStatement *pstmt;

    pstmt = connection->prepareStatement("INSERT INTO Utenti (userid,salt,hashedPassword) VALUES(?,?,?)");
    try{
        pstmt->setString(1,userid);
        pstmt->setString(2,strSalt);
        pstmt->setString(3,hashedPass);
        pstmt->executeUpdate();
        connection->commit();
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
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
        pstmt->setUInt(4,ProceduraVoto::statiProcedura::creazione);
        pstmt->setDateTime(5,currentTime);
        pstmt->setDateTime(6,procedura->getData_ora_inizio());
        pstmt->setDateTime(7,procedura->getData_ora_termine());
        pstmt->executeUpdate();
        connection->commit();
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
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
        cerr << "Exception occurred: "<< ex.getErrorCode()<<endl;
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
        cerr << "Exception occurred: "<< ex.getErrorCode()<<endl;
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


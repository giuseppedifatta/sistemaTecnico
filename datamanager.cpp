#include "datamanager.h"
//#include "mysql_driver.h"

//#ifndef XMLCheckResult
//#define XMLCheckResult(a_eResult)
//#endif

#include <istream>
#include <QDebug>
#include "openotp_login.h"
#include "conf.h"


DataManager::DataManager(QObject *parent) : QObject(parent)
{
    try{
        driver=get_driver_instance();
        string ip_port = getConfig("pathDBmydb");
        string userMydb = getConfig("userDBmydb");
        string passMydb = getConfig("passDBmydb");
        string nameMydb = getConfig("nameDBmydb");
        connection = driver->connect(ip_port,userMydb, passMydb);
        connection->setAutoCommit(false);
        connection->setSchema(nameMydb);
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }

    try{
        driver=get_driver_instance();
        string ip_port = getConfig("pathDBanagrafica");
        string userAnagrafica = getConfig("userDBanagrafica");
        string passAnagrafica = getConfig("passDBanagrafica");
        string nameAnagrafica = getConfig("nameDBanagrafica");
        connectionAnagrafica=driver->connect(ip_port,userAnagrafica, passAnagrafica);
        connectionAnagrafica->setAutoCommit(false);
        connectionAnagrafica->setSchema(nameAnagrafica);
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
    connection->close();
    delete connection;
    connectionAnagrafica->close();
    delete connectionAnagrafica;
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

    string descrizioneElezione = scheda->getDescrizioneElezione();
    pElement = xmlDoc.NewElement("descrizioneElezione");
    pElement->SetText(descrizioneElezione.c_str());
    pRoot->InsertEndChild(pElement);

    uint numPref = scheda->getNumPreferenze();
    pElement = xmlDoc.NewElement("numeroPreferenze");
    pElement->SetText(numPref);
    pRoot->InsertEndChild(pElement);

    XMLNode * pTipiVotanti = xmlDoc.NewElement("tipiVotanti");
    pRoot->InsertEndChild(pTipiVotanti);
    vector <uint> idTipiVotanti = scheda->getIdTipiVotantiConsentiti();
    for (uint i = 0; i < idTipiVotanti.size(); i++){
        XMLElement * elementIdTipo = xmlDoc.NewElement("idTipoVotanti");
        elementIdTipo->SetText(idTipiVotanti.at(i));
        pTipiVotanti->InsertEndChild(elementIdTipo);
    }


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
            XMLElement * pElement = xmlDoc.NewElement("candidato");
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
        cout << "Procedura: " << scheda->getIdProceduraVoto() << ", schede inserite: " << numSchedeInserite << endl;
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

    //se il numero di schede è quello richiesto, aggiorniamo lo stato della procedura su: programmata e il numero di schede inserite al numero di schede contate sull'apposita tabella nella query precedente
    if(numSchedeInserite==numSchedeRichieste){
        cout << "Aggiornamento numero schede inserite e stato della procedura su <programmata>" << endl;
        PreparedStatement * pstmt;
        pstmt = connection->prepareStatement("UPDATE `ProcedureVoto` SET stato=?,ultimaModifica=?, schedeInserite=? WHERE idProceduraVoto=?");
        try{
            pstmt->setUInt(1,ProceduraVoto::statiProcedura::programmata);
            pstmt->setDateTime(2,this->currentTimeDbFormatted());
            pstmt->setUInt(3,numSchedeInserite);
            pstmt->setUInt(4,scheda->getIdProceduraVoto());
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
        pstmt = connection->prepareStatement("UPDATE `ProcedureVoto` SET schedeInserite=?,ultimaModifica=? WHERE `idProceduraVoto`=?");
        try{

            pstmt->setUInt(1,numSchedeInserite);
            pstmt->setDateTime(2,this->currentTimeDbFormatted());
            pstmt->setUInt(3,scheda->getIdProceduraVoto());
            pstmt->executeUpdate();
            connection->commit();
        }catch(SQLException &ex){
            cerr << "Exception occurred: " << ex.getErrorCode() <<endl;
        }
        pstmt->close();
        delete pstmt;
    }
    emit storedSchedaVoto();
}

void DataManager::storeRP(ResponsabileProcedimento *rp)
{
    string userid;
    string vergin_userid = rp->getNome() + "." + rp->getCognome();
    //rimuoviamo gli spazi
    remove_copy( vergin_userid.begin() , vergin_userid.end() , back_inserter( userid ) , ' ');
    //trasformiamo in soli caratteri minuscoli
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
            if(resultSet->next()){
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
    AutoSeededRandomPool rng;

    RSA::PrivateKey rsaPrivate;
    rsaPrivate.GenerateRandomWithKeySize(rng, 3072);

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

    //copio la chiave privata in un buffer ByteQueue
    ByteQueue queue2;
    rsaPrivate.Save(queue2);

    //copiamo la privateKey dal ByteQueue in una stringa
    string privateKey;
    StringSink ss2(privateKey);
    queue2.CopyTo(ss2);
    ss2.MessageEnd();
    cout << "privateKey: " << privateKey << endl; //formato byte

    //calcoliamo dalla password scelta dall'RP una derivedKey per cifrare la privateKey di RP
    string pass = rp->getPassword();
    cout <<"Password RP: " << pass << endl;
    /*Generate salt */
    //AutoSeededRandomPool prng;
    SecByteBlock salt(AES::BLOCKSIZE);
    rng.GenerateBlock(salt, sizeof(salt));
    string strSalt;
    HexEncoder hex(new StringSink(strSalt));
    hex.Put(salt, salt.size());
    hex.MessageEnd();

    string derivedKeyEncoded = deriveKeyFromPass(pass,strSalt);
    cout << "derivedKey ottenuta dalla password di RP: " << derivedKeyEncoded << endl;


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
    byte iv[AES::BLOCKSIZE];
    memset(iv, 0x01,AES::BLOCKSIZE);


    //cifriamo la chiave privata di RP con chiave simmetrica
    string encodedPrivateKeyRPCifrata = AESencryptStdString(privateKey,key,iv);

    cout << "privateKey cifrata encoded: " << encodedPrivateKeyRPCifrata << endl;
    //chiave privata cifrata pronta per essere memorizzata sul DB
    std::stringstream rsaPrivateBlob(encodedPrivateKeyRPCifrata);

    pstmt=connection->prepareStatement
            ("INSERT INTO ResponsabiliProcedimento ( userid, nome, cognome, dataNascita, luogoNascita,publicKey,encryptedPrivateKey,salt ) VALUES (?,?,?,?,?,?,?,?)");
    try{
        cout << "Memorizzazione del nuovo RP in corso..." << endl;
        pstmt->setString(1,userid);
        pstmt->setString(2,rp->getNome());
        pstmt->setString(3,rp->getCognome());
        pstmt->setDateTime(4,rp->getDataNascita());
        pstmt->setString(5,rp->getLuogoNascita());
        pstmt->setBlob(6,&rsaPublicBlob);
        pstmt->setBlob(7,&rsaPrivateBlob);
        pstmt->setString(8,strSalt);
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
string DataManager::AESencryptStdString(string plain, SecByteBlock key, byte* iv) {


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
        //cout << "plain text: " << plain << endl;

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
    // Pretty print
    string encodedCipher;
    StringSource(cipher, true,
                 new HexEncoder(
                     new StringSink(encodedCipher)
                     ) // HexEncoder
                 ); // StringSource
    //cout << "cipher text encoded: " << encodedCipher << endl;

    return encodedCipher;
}

string DataManager::AESencryptStdString(string plain, SecByteBlock key, SecByteBlock  iv) {


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
        //cout << "plain text: " << plain << endl;

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
    string encodedCipher;
    StringSource(cipher, true,
                 new HexEncoder(
                     new StringSink(encodedCipher)
                     ) // HexEncoder
                 ); // StringSource
    //cout << "cipher text encoded: " << encodedCipher << endl;

    return encodedCipher;
}

string DataManager::AESdecryptStdString(string encodedCipher, SecByteBlock key, byte* iv){
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

    string cipher;
    StringSource(encodedCipher,true,
                 new HexDecoder(
                     new StringSink(cipher)
                     ) // HexEncoder
                 ); // StringSource
    cout << "cipher: " << cipher << endl;
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

string DataManager::AESdecryptStdString(string encodedCipher, SecByteBlock key, SecByteBlock iv){
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

    string cipher;
    StringSource(encodedCipher,true,
                 new HexDecoder(
                     new StringSink(cipher)
                     ) // HexEncoder
                 ); // StringSource
    cout << "cipher: " << cipher << endl;
    try
    {

        CBC_Mode< AES >::Decryption aesDecryptor;
        aesDecryptor.SetKeyWithIV(key, key.size(), iv);

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource (encodedCipher, true,
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

string DataManager::generaSharedKey()
{
    string sharedKey;

    AutoSeededRandomPool rng;
    SecByteBlock key(16);
    rng.GenerateBlock( key, key.size() );

    StringSource(key, key.size(), true,
                 new HexEncoder(
                     new StringSink(sharedKey)
                     ) // HexEncoder
                 ); // StringSource


    return sharedKey;
}


string DataManager::deriveKeyFromPass(string password, string salt){
    string encodedDerivedKey;
    try {

        // KDF parameters
        unsigned int iterations = 15000;
        char purpose = 0; // unused by Crypto++

        // 32 bytes of derived material. Used to key the cipher.
        //   16 bytes are for the key, and 16 bytes are for the iv.
        SecByteBlock derived(AES::MAX_KEYLENGTH);

        // KDF function
        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> kdf;
        kdf.DeriveKey(derived.data(), derived.size(), purpose, (byte*)password.data(), password.size(), (byte *) salt.data(), salt.size(), iterations);




        // Encode derived
        HexEncoder hex(new StringSink(encodedDerivedKey));
        hex.Put(derived.data(), derived.size());
        hex.MessageEnd();

        // Print stuff
        cout << "pass: " << password << endl;
        cout << "derived key: " << encodedDerivedKey << endl;

    }
    catch(CryptoPP::Exception& ex)
    {
        cerr << ex.what() << endl;
    }
    return encodedDerivedKey;
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

                pstmt2 = connection->prepareStatement("UPDATE ProcedureVoto SET stato=?,ultimaModifica=? WHERE idProceduraVoto=?");
                try{
                    pstmt2->setUInt(1,statoProceduraAggiornato); //in_corso
                    pstmt2->setDateTime(2,this->currentTimeDbFormatted());
                    pstmt2->setUInt(3,idProceduraVoto);

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
    pstmt = connection->prepareStatement("SELECT * FROM Sessioni WHERE idProceduraVoto = ? order by data ASC, apertura ASC");
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

void DataManager::getTipiVotanti()
{
    vector <TipoVotante> tipiVotanti;
    PreparedStatement * pstmt;
    ResultSet * resultSet;
    pstmt = connectionAnagrafica->prepareStatement("SELECT * FROM TipiVotanti");
    try{
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            TipoVotante t;

            uint id = resultSet->getUInt("idTipoVotanti");
            t.setId(id);

            string descrizione = resultSet->getString("tipoVotanti");
            t.setDescrizione(descrizione);
            tipiVotanti.push_back(t);
        }
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;
    resultSet->close();
    delete resultSet;


    emit readyTipiVotanti(tipiVotanti);
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

            XMLText* textNodeDescrizioneElezione= rootNode->FirstChildElement("descrizioneElezione")->FirstChild()->ToText();
            string descrizioneElezione = textNodeDescrizioneElezione->Value();
            cout << "descrizione elezione: " << descrizioneElezione << endl;
            s.setDescrizioneElezione(descrizioneElezione);

            XMLText* textNodeNumeroPreferenze = rootNode->FirstChildElement("numeroPreferenze")->FirstChild()->ToText();
            uint numeroPreferenze = atoi(textNodeNumeroPreferenze->Value());
            cout << "Numero preferenze: " << numeroPreferenze << endl;
            s.setNumPreferenze(numeroPreferenze);

            //parsing degli idTipiVotanti
            XMLElement * tipiVotantiElement = rootNode->FirstChildElement("tipiVotanti");

            XMLElement * firstIdTipoVotantiElement = tipiVotantiElement->FirstChildElement("idTipoVotanti");
            XMLElement * lastIdTipoVotantiElement = tipiVotantiElement->LastChildElement("idTipoVotanti");

            XMLElement *idTipoVotantiElement = firstIdTipoVotantiElement;
            bool lastIdTipoVotanti = false;
            do{

                XMLText* textNodeIdTipoVotanti = idTipoVotantiElement->FirstChild()->ToText();
                uint idTipoVotanti = atoi(textNodeIdTipoVotanti->Value());
                cout << "Id tipo Votanti: " << idTipoVotanti << endl;
                s.addIdTipiVotantiConsentiti(idTipoVotanti);


                if(idTipoVotantiElement == lastIdTipoVotantiElement){
                    lastIdTipoVotanti = true;
                }
                else{
                    //accediamo alla successiva lista nella scheda di voto
                    idTipoVotantiElement = idTipoVotantiElement->NextSiblingElement("idTipoVotanti");
                    cout << "ottengo il puntatore al successivo idTipoVotanti" << endl;
                }
            }while(!lastIdTipoVotanti);
            cout << "non ci sono altri idTipoVotanti" << endl;

            //parsing delle liste
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
    resultSet->close();
    delete pstmt;
    delete resultSet;

    if (availability){
        emit proceduraRangeAvailable(inizio,termine);
    }
    else{
        emit requestedProceduraRangeInUse();
    }
}

void DataManager::getInfoSeggi()
{
    vector <InfoSeggio> seggiPresenti;
    PreparedStatement * pstmt;
    ResultSet * resultSet;
    pstmt = connection->prepareStatement("SELECT * FROM Seggi");
    try{
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            InfoSeggio is;
            uint idSeggio = resultSet->getUInt("idSeggio");
            is.setIdSeggio(idSeggio);
            string sede = resultSet->getString("sede");
            is.setDescrizione(sede);
            string ip = resultSet->getString("ipSeggio");
            is.setIp(ip);
            seggiPresenti.push_back(is);
        }

    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }


    pstmt->close();
    resultSet->close();
    delete pstmt;
    delete resultSet;

    emit readyInfoSeggi(seggiPresenti);
}

void DataManager::deleteSeggio(uint idSeggio)
{
    PreparedStatement * pstmt;
    pstmt = connection->prepareStatement("DELETE FROM Seggi WHERE idSeggio = ?");
    try{
        pstmt->setUInt(1,idSeggio);
        pstmt->executeUpdate();
        connection->commit();

    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }


    pstmt->close();
    delete pstmt;

    getInfoSeggi();
}

void DataManager::addPostazioniNoCommit(uint idSeggio, vector<string> ipPostazioni, string descrizioneSeggio)
{
    PreparedStatement *pstmt;
    string ipSeggio = ipPostazioni.at(0);
    pstmt = connection->prepareStatement("INSERT INTO Seggi (ipSeggio,sede,idSeggio) VALUES(?,?,?)");
    try{
        pstmt->setString(1,ipSeggio);
        pstmt->setString(2,descrizioneSeggio);
        pstmt->setUInt(3,idSeggio);
        pstmt->executeUpdate();
        //commit rimandata al momento dell'inserimento degli hardware token

    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    delete pstmt;

    //    //se vogliamo un idSeggio di tipo AUTO INCREMENT, decommentare la sezione per ottenere l'id del seggio appena aggiunto
    //    pstmt = connection->prepareStatement("SELECT LAST_INSERT_ID() AS idSeggio");
    //    ResultSet *resultSet;
    //    uint idSeggio;
    //    try{
    //        resultSet = pstmt->executeQuery();
    //        resultSet->next();//accediamo al primo e unico valore del resultSet
    //        idSeggio = resultSet->getUInt("idSeggio");
    //        cout << "id Seggio: " << idSeggio << endl;
    //    }catch(SQLException &ex){
    //        cerr << "Exception occurred: "<< ex.getErrorCode()<<endl;
    //    }
    //    pstmt->close();
    //    resultSet->close();
    //    delete pstmt;
    //    delete resultSet;


    for (uint i = 0; i <=3 ; i++){
        pstmt = connection->prepareStatement("INSERT INTO Postazioni (idSeggio,ipPostazione) VALUES(?,?)");
        try{
            pstmt->setUInt(1,idSeggio);
            pstmt->setString(2,ipPostazioni.at(i));
            pstmt->executeUpdate();
            //commit rimandata al momento dell'inserimento degli hardware token

        }catch(SQLException &ex){
            cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
        }
        pstmt->close();
        delete pstmt;
    }

    emit idSeggioCreating(idSeggio);

}

void DataManager::rollbackSeggio()
{
    try{
        connection->rollback();
    }
    catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    emit abortedSeggio();
}

void DataManager::commitSeggio()
{
    //facciamo la commit di tutte le informazioni del nuovo seggio, memorizzate ma non confermate con la commit
    try{
        connection->commit();
        cout << "Nuovo Seggio creato" << endl;
    }
    catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    emit storedSeggio();
}

void DataManager::testTokenAndStoreNoCommit(string sn, string user, string pass, string otp, uint idSeggio)
{
    //controlliamo se il token è già utilizzato per un altro seggio, in questo caso i passaggi successivi vanno saltati
    bool tokenAlredyUsed = true;

    PreparedStatement *pstmt;
    ResultSet *resultSet;
    pstmt = connection->prepareStatement("SELECT snToken FROM Token WHERE snToken = ?");
    try{
        pstmt->setString(1,sn);
        resultSet = pstmt->executeQuery();
        if(!(resultSet->next())){
            tokenAlredyUsed = false;
        }
    }
    catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }
    pstmt->close();
    resultSet->close();
    delete pstmt;
    delete resultSet;

    if(tokenAlredyUsed){
        emit tokenNotAvailable();
        return;
    }

    //TODO test token
    bool testOk = false;

    testOk = this->validateOTP(user,pass,otp);



    //se il test non ha avuto successo, interrompiamo
    if(!testOk){
        emit testTokenFail();
        return;
    }

    //memorizzazione hardware token
    pstmt = connection->prepareStatement("INSERT INTO Token (snToken,username,password,idSeggio) VALUES(?,?,?,?)");
    try{
        pstmt->setString(1,sn);
        pstmt->setString(2,user);
        pstmt->setString(3,pass);
        pstmt->setUInt(4,idSeggio);
        pstmt->executeUpdate();
        //commit rimandata al completamento del seggio
    }
    catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }

    pstmt->close();
    delete pstmt;

    emit tokenStored(sn, user,pass, idSeggio);

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
    ResultSet *resultSet;
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
    resultSet->close();
    delete resultSet;


    //----generazione chiavi di sessione per le postazioni presenti----
    //1. ottieni id delle sessioni
    pstmt = connection->prepareStatement("SELECT idSessione FROM Sessioni WHERE idProceduraVoto=?");
    vector <uint> idSessioni;
    try{

        pstmt->setUInt(1,idProceduraInserita);
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            idSessioni.push_back(resultSet->getUInt("idSessione"));
        }

    }catch(SQLException &ex){
        cerr << "Exception occurred: "<< ex.getErrorCode()<<endl;
    }

    pstmt->close();
    delete pstmt;
    resultSet->close();
    delete resultSet;

    //ottieni id delle postazioni
    pstmt = connection->prepareStatement("SELECT idPostazione FROM Postazioni");
    vector <uint> idPostazioni;
    try{
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            idPostazioni.push_back(resultSet->getUInt("idPostazione"));
        }

    }catch(SQLException &ex){
        cerr << "Exception occurred: "<< ex.getErrorCode()<<endl;
    }

    pstmt->close();
    delete pstmt;
    resultSet->close();
    delete resultSet;



    //3.per ogni sessione e per ogni postazione genera una chiave e memorizzala sul db
    pstmt = connection->prepareStatement("INSERT INTO ChiaviSessione (idPostazione,idSessione,sharedKey) VALUES(?,?,?)");
    string sharedKey;
    for (uint s=0; s< idSessioni.size(); s++){
        for (uint p = 0; p < idPostazioni.size();p++ ){

            try{
                sharedKey = this->generaSharedKey();
                pstmt->setUInt(1,idPostazioni.at(p));
                pstmt->setUInt(2,idSessioni.at(s));
                pstmt->setString(3,sharedKey);
                pstmt->executeUpdate();
                connection->commit();

            }catch(SQLException &ex){
                cerr << "Exception occurred: "<< ex.getErrorCode()<<endl;
            }


        }

    }
    pstmt->close();
    delete pstmt;

    emit storedProcedura();
}

string DataManager::hashPassword( string plainPass, string salt){
    SecByteBlock result(32);
    string hexResult;

    PKCS5_PBKDF2_HMAC<SHA256> pbkdf;

    pbkdf.DeriveKey(result, result.size(),0x00,(byte *) plainPass.data(), plainPass.size(),(byte *) salt.data(), salt.size(),10000);

    //ArraySource resultEncoder(result,result.size(), true, new HexEncoder(new StringSink(hexResult)));

    HexEncoder hex(new StringSink(hexResult));
    hex.Put(result.data(), result.size());
    hex.MessageEnd();

    return hexResult;

}

string DataManager::currentTimeDbFormatted() {
    time_t now = time(0);
    string dt  = ctime(&now);
    tm *ltm = localtime(&now);
    //	int anno = ltm->tm_year +1900;
    //	int mese = ltm->tm_mon + 1;
    //	int day = ltm->tm_mday;
    char buffer[20];
    //date formatted for sql db comparing
    strftime(buffer,20,"%Y-%m-%d %X",ltm); //%F equivalent to %Y-%m-%d 2001-08-23 , %X equivalent to %T 14:55:02
    string currentTime = buffer;
    return currentTime;
}


bool DataManager::validateOTP(string user,string pass,string otpStr){

    //contattare otpServer per verificare il token rispetto all'account relativo al token associato alla postazione voto
    //string url = "https://147.163.26.229:8443/openotp/";
    string url = getConfig("serviceOTP");
    string username = user;
    string password = pass;

    char * writableURL = new char[url.size() + 1];
    std::copy(url.begin(), url.end(), writableURL);
    writableURL[url.size()] = '\0'; // don't forget the terminating 0

    char * writableUsername = new char[username.size() + 1];
    std::copy(username.begin(), username.end(), writableUsername);
    writableUsername[username.size()] = '\0'; // don't forget the terminating 0

    char * writablePassword = new char[password.size() + 1];
    std::copy(password.begin(), password.end(), writablePassword);
    writablePassword[password.size()] = '\0'; // don't forget the terminating 0


    char * writableOTP = new char[otpStr.size() + 1];
    std::copy(otpStr.begin(), otpStr.end(), writableOTP);
    writableOTP[otpStr.size()] = '\0'; // don't forget the terminating 0

    bool success = otp_login(writableURL,writableUsername,writablePassword,writableOTP);
    delete[] writableURL;
    delete[] writableUsername;
    delete[] writablePassword;
    delete[] writableOTP;

    return success;

}


void DataManager::getSessionKeys(uint idProcedura){
    PreparedStatement * pstmt;
    ResultSet *resultSet;
    vector<ChiaveSessione> sessionKeys;

    string stmt = "SELECT p.ipPostazione , p.idSeggio, p.Sede, sk.sharedKey, sk.idSessione, sk.`data`, sk.apertura, sk.chiusura "
                  "FROM (SELECT pp.idSeggio, pp.idPostazione, pp.ipPostazione, Seggi.Sede "
                  "FROM Seggi INNER JOIN Postazioni pp ON pp.idSeggio=Seggi.idSeggio) p "
                  "INNER JOIN(SELECT  cs.idSessione, cs.sharedKey, cs.idPostazione, s.`data`, s.apertura, s.chiusura	"
                  "FROM ChiaviSessione cs	INNER JOIN (SELECT idSessione, `Sessioni`.`data`, apertura,chiusura  "
                  "FROM Sessioni WHERE idProceduraVoto =  ?) s "
                  "ON cs.idSessione = s.idSessione) sk ON p.idPostazione = sk.idPostazione ORDER BY idSeggio, idSessione, p.idPostazione";
    pstmt = connection->prepareStatement(stmt);
    try{
        pstmt->setUInt(1,idProcedura);
        resultSet = pstmt->executeQuery();
        while(resultSet->next()){
            ChiaveSessione cs;
            string ipPostazione = resultSet->getString("ipPostazione");
            cs.setIpPostazione(ipPostazione);
            uint idSeggio = resultSet->getUInt("idSeggio");
            cs.setIdSeggio(idSeggio);
            string sedeSeggio = resultSet->getString("sede");
            cs.setSedeSeggio(sedeSeggio);
            string sharedKey = resultSet->getString("sharedKey");
            cs.setSharedKey(sharedKey);

            SessioneVoto sv;
            QString qsData = QString::fromStdString(resultSet->getString("data")); //format is: yyyy-MM-dd
            QDate data = QDate::fromString(qsData,"yyyy-MM-dd");
            sv.setData(data.toString("dd/MM/yyyy").toStdString());

            QString qsApertura = QString::fromStdString(resultSet->getString("apertura")); //format is: yyyy-MM-dd
            QTime tApertura = QTime::fromString(qsApertura,"hh:mm:ss");
            sv.setOraApertura(tApertura.toString("hh:mm").toStdString());

            QString qsChiusura = QString::fromStdString(resultSet->getString("chiusura")); //format is: yyyy-MM-dd
            QTime tChiusura = QTime::fromString(qsChiusura,"hh:mm:ss");
            sv.setOraChiusura(tChiusura.toString("hh:mm").toStdString());

            sv.setIdSessione(resultSet->getUInt("idSessione"));
            cs.setSessioneVoto(sv);

            sessionKeys.push_back(cs);

        }
    }catch(SQLException &ex){
        cerr << "Exception occurred: "<<ex.getErrorCode()<<endl;
    }

    emit readyChiaviSessione(sessionKeys);
}

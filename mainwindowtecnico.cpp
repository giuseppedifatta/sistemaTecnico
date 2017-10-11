#include "mainwindowtecnico.h"
#include "ui_mainwindowtecnico.h"
#include <QPainter>


using namespace std;

MainWindowTecnico::MainWindowTecnico(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowTecnico)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
    this->setTables();

    ui->lineEdit_password_tecnico->setEchoMode(QLineEdit::Password);
    ui->lineEdit_new_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_repeat_new_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_su_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_password_rp->setEchoMode(QLineEdit::Password);
    ui->lineEdit_ripeti_password_rp->setEchoMode(QLineEdit::Password);

    cout << "inizializzazione model..." << endl;
    model = new DataManager(this);
    idNuovoSeggio = 0;
    numHTaggiunti = 0;
    this->numTipoVotantiChecked = 0;

    QObject::connect(this,SIGNAL(tecnicoPass(QString)),model,SLOT(checkPassTecnico(QString)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(passOK()),this,SLOT(showViewSceltaOperazione()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(wrongTecnicoPass()),this,SLOT(passwordErrorMessage()),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(changeTecnicoPass(QString,QString)),model,SLOT(tryChangeTecnicoPass(QString,QString)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(wrongSUpass()),this,SLOT(suPassErrorMessage()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(tecnicoPassChanged()),this,SLOT(tecnicoPassAggiornata()),Qt::QueuedConnection);

    QObject::connect(this,SIGNAL(schedaPronta(SchedaVoto*)),model,SLOT(storeScheda(SchedaVoto*)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(storedSchedaVoto()),this,SLOT(messageStoredSchedaVoto()),Qt::QueuedConnection);

    QObject::connect(this,SIGNAL(rpPronto(ResponsabileProcedimento*)),model,SLOT(storeRP(ResponsabileProcedimento*)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(storedRP(QString)),this,SLOT(messageRegisteredRP(QString)),Qt::QueuedConnection);

    QObject::connect(this,SIGNAL(needInfoRPS()),model,SLOT(getRPSFromDB()),Qt::QueuedConnection);
    qRegisterMetaType< vector<ResponsabileProcedimento> >( "vector<ResponsabileProcedimento>" );
    QObject::connect(model,SIGNAL(readyRPS(vector <ResponsabileProcedimento>)),this,SLOT(startCreationProcedura(vector <ResponsabileProcedimento>)),Qt::QueuedConnection);

    QObject::connect(this,SIGNAL(proceduraPronta(ProceduraVoto*)),model,SLOT(storeProcedura(ProceduraVoto*)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(storedProcedura()),this,SLOT(messageStoredProcedura()),Qt::QueuedConnection);

    QObject::connect(this,SIGNAL(needInfoProcedureVoto()),model,SLOT(getProcedureVotoFromDB()),Qt::QueuedConnection);
    qRegisterMetaType< QList<ProceduraVoto> >( "QList<ProceduraVoto>" );
    QObject::connect(model,SIGNAL(readyProcedure(QList<ProceduraVoto>)),this,SLOT(showViewProcedureVoto(QList<ProceduraVoto>)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(deleteProcedura(uint)),model,SLOT(deleteProceduraVoto(uint)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(deletedProcedura()),this,SLOT(on_pushButton_visualizza_procedure_clicked()),Qt::QueuedConnection);

    qRegisterMetaType< QList <SessioneVoto> >("QList <SessioneVoto>");
    QObject::connect(this,SIGNAL(needSessioni(uint)),model,SLOT(getSessioniProceduraFromDB(uint)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(readySessioni(QList <SessioneVoto>)),this,SLOT(showViewSessioniProcedura(QList <SessioneVoto>)),Qt::QueuedConnection);

    qRegisterMetaType< QList <SchedaVoto> >("QList <SchedaVoto>");
    QObject::connect(this,SIGNAL(needSchedeProcedura(uint)),model,SLOT(getSchedeProceduraFromDB(uint)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(readySchede(QList<SchedaVoto>)),this,SLOT(showViewSchedeProcedura(QList<SchedaVoto>)),Qt::QueuedConnection);

    QObject::connect(this,SIGNAL(checkRangeProcedura(QDateTime,QDateTime)),model,SLOT(checkAvailabilityProceduraRange(QDateTime,QDateTime)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(proceduraRangeAvailable(QDateTime,QDateTime)),this,SLOT(setPeriodoProcedura(QDateTime,QDateTime)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(requestedProceduraRangeInUse()),this,SLOT(messageProceduraRangeInUse()),Qt::QueuedConnection);

    qRegisterMetaType< vector <InfoSeggio> >("vector <InfoSeggio>");
    QObject::connect(this,SIGNAL(needInfoSeggi()),model,SLOT(getInfoSeggi()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(readyInfoSeggi(vector<InfoSeggio>)),this,SLOT(showViewSeggi(vector<InfoSeggio>)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(seggioToDelete(uint)),model,SLOT(deleteSeggio(uint)),Qt::QueuedConnection);

    qRegisterMetaType< vector <string> >("vector <string>");
    qRegisterMetaType<string>("string");
    QObject::connect(this, SIGNAL(postazioniToAdd(vector <string>,string)),model,SLOT(addPostazioniNoCommit(vector<string>,string)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(idSeggioCreating(uint)),this,SLOT(addGeneratoriOTP(uint)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(rollbackNuovoSeggio()),model,SLOT(rollbackSeggio()),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(commitNuovoSeggio()),model,SLOT(commitSeggio()),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(testAndRecord(string,string,string,string,uint)),model,SLOT(testTokenAndStoreNoCommit(string,string,string,string,uint)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(abortedSeggio()),this,SLOT(showMessageCreazioneSeggioAnnullata()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(storedSeggio()),this,SLOT(showMessaggeSeggioCreato()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(tokenStored(string,string,string,uint)),this,SLOT(addTokenToTable(string,string,string,uint)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(tokenNotAvailable()),this,SLOT(showMessageTokenNotAvailable()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(testTokenFail()),this,SLOT(showMessageTestTokenFail()),Qt::QueuedConnection);

    qRegisterMetaType<vector <TipoVotante> >("vector <TipoVotante>");
    QObject::connect(model,SIGNAL(readyTipiVotanti(vector<TipoVotante>)),this,SLOT(createScheda(vector<TipoVotante>)),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(needTipiVotanti()),model,SLOT(getTipiVotanti()),Qt::QueuedConnection);
}


MainWindowTecnico::~MainWindowTecnico()
{
    delete model;
    delete ui;
}

void MainWindowTecnico::setTables(){
    QStringList tableHeaders;
    tableHeaders << "seleziona" << "id Procedura" << "Descrizione" << "id RP" << "Inizio" << "Termine" << "Schede Richieste" << "Schede Inserite" << "Stato Procedura" ;

    ui->tableWidget_lista_procedure->setHorizontalHeaderLabels(tableHeaders);
    //ui->tableWidget_lista_procedure->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_lista_procedure->resizeColumnsToContents();
    ui->tableWidget_lista_procedure->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_lista_procedure->setFocusPolicy(Qt::NoFocus);

    QStringList tableHeaders2;
    tableHeaders2 << "id Sessione" << "data Sessione" << "apertura Seggi" << "chiusura Seggi";
    ui->tableWidget_sessioni->setHorizontalHeaderLabels(tableHeaders2);
    ui->tableWidget_sessioni->resizeColumnsToContents();
    ui->tableWidget_sessioni->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_sessioni->setFocusPolicy(Qt::NoFocus);
}




void MainWindowTecnico::initTableHT(){
    ui->tableWidget_hardwareToken->verticalHeader()->setVisible(false);
    ui->tableWidget_hardwareToken->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_hardwareToken->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList tableHeaders;
    tableHeaders << "S/N hardware token" << "username" << "password" << "id Seggio" ;

    ui->tableWidget_hardwareToken->setHorizontalHeaderLabels(tableHeaders);

    ui->tableWidget_hardwareToken->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tableWidget_hardwareToken->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableWidget_hardwareToken->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);

    ui->tableWidget_hardwareToken->model()->removeRows(0,ui->tableWidget_hardwareToken->rowCount());
}

void MainWindowTecnico::on_pushButton_exit_clicked()
{
    QApplication::quit();
}

void MainWindowTecnico::on_pushButton_login_tecnico_clicked()
{
    ui->label_password_errata->hide();
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginTecnicoPassword);
}

void MainWindowTecnico::on_pushButton_change_password_clicked()
{
    ui->label_error_su_password->hide();
    ui->label_error_repeat_pass->hide();
    ui->pushButton_confirm_new_password->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::modificaPasswordTecnico);

}

void MainWindowTecnico::on_pushButton_back_to_login_urna_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
    ui->label_password_errata->hide();
    ui->lineEdit_password_tecnico->setText("");

}

void MainWindowTecnico::on_pushButton_accedi_clicked()
{
    QString pass = ui->lineEdit_password_tecnico->text();
    emit tecnicoPass(pass);
}

void MainWindowTecnico::showViewSceltaOperazione()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    ui->lineEdit_password_tecnico->setText("");

}

void MainWindowTecnico::passwordErrorMessage()
{
    ui->label_password_errata->show();
}

void MainWindowTecnico::suPassErrorMessage()
{
    ui->label_error_su_password->show();
    ui->pushButton_confirm_new_password->setEnabled(false);
}

void MainWindowTecnico::tecnicoPassAggiornata()
{

    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
    ui->lineEdit_su_password->clear();
    ui->lineEdit_new_password->clear();
    ui->lineEdit_repeat_new_password->clear();
}

void MainWindowTecnico::messageStoredSchedaVoto()
{
    QMessageBox::information(this,"Success","La scheda di voto è stata correttamente memorizzata nella procedura di voto selezionata.");

    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaProcedure);

    delete nuovaScheda;

    pulisciInterfacciaCreazioneScheda();

    on_pushButton_visualizza_procedure_clicked();
}

void MainWindowTecnico::messageStoredProcedura()
{
    QMessageBox::information(this,"Success","La procedura è stata correttamente salvata.");
    delete nuovaProcedura;
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    pulisciInterfacciaCreazioneProcedura();
}

void MainWindowTecnico::messageRegisteredRP(QString userid)
{
    QMessageBox::information(this,"Success","Il Responsabile di Procedimento è stato correttamente registrato. La sua userid da conservare per l'accesso è: " + userid);
    delete nuovoRP;
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    pulisciInterfacciaCreazioneRP();
}

void MainWindowTecnico::startCreationProcedura(vector<ResponsabileProcedimento> rps)
{
    //aggiungere le info ricevute in rps alla struttura nuovaProcedura nel dato membro rps
    if(rps.size()>0){
        nuovaProcedura->setRps(rps);
        for (unsigned int i = 0; i < rps.size(); i++){
            QString rp = /*QString::number(rps.at(i).getIdRP()) + ", " + */QString::fromStdString(rps.at(i).getNome()) + " " + QString::fromStdString(rps.at(i).getCognome());
            ui->comboBox_idRP->addItem(rp);
        }
        ui->pushButton_visualizzaInfoRP->setEnabled(true);
    }
    else{
        ui->pushButton_visualizzaInfoRP->setEnabled(false);
    }


    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::creazioneProcedura);
}

void MainWindowTecnico::on_pushButton_back_to_login_urna_2_clicked()
{
    ui->lineEdit_su_password->clear();
    ui->lineEdit_new_password->clear();
    ui->lineEdit_repeat_new_password->clear();
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
}

void MainWindowTecnico::on_pushButton_confirm_new_password_clicked()
{
    QString newPassTecnico = ui->lineEdit_new_password->text();
    QString repeat_pass = ui->lineEdit_repeat_new_password->text();
    if(newPassTecnico==repeat_pass){
        QString su_pass = ui->lineEdit_su_password->text();
        emit changeTecnicoPass(su_pass, newPassTecnico);
    }
    else{
        ui->label_error_repeat_pass->show();
    }


}



void MainWindowTecnico::on_pushButton_crea_procedura_clicked()
{
    ui->pushButton_salva_procedura->setEnabled(false);
    ui->dateEdit_data_sessione->setEnabled(false);
    ui->timeEdit_apertura_sessione->setEnabled(false);
    ui->timeEdit_chiusura_sessione->setEnabled(false);
    ui->pushButton_aggiungi_sessione->setEnabled(false);
    ui->pushButton_elimina_sessione->setEnabled(false);

    nuovaProcedura = new ProceduraVoto();
    QDateTime tomorrow(QDateTime::currentDateTime().date().addDays(1));
    ui->dateTimeEdit_data_ora_inizio->setMinimumDateTime(tomorrow);

    //TODO emetto il segnale per la richiesta delle info degli RP memorizzati nel sistema
    emit needInfoRPS();


}

void MainWindowTecnico::on_pushButton_registra_RP_clicked()
{
    nuovoRP = new ResponsabileProcedimento();
    ui->dateEdit_data_nascita_rp->clear();
    ui->dateEdit_data_nascita_rp->setMaximumDate(QDate::currentDate().addYears(-18));
    ui->label_error_password_rp->hide();
    ui->pushButton_completa_reg_rp->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::registrazioneRP);
}

void MainWindowTecnico::on_pushButton_visualizza_procedure_clicked()
{
    idProceduraSelezionata = -1;
    statoProceduraSelezionata = ProceduraVoto::statiProcedura::undefined;
    emit needInfoProcedureVoto();

}

void MainWindowTecnico::showViewProcedureVoto(QList <ProceduraVoto> procedureVoto){
    //pulizia tabella - tipo 1
    ui->tableWidget_lista_procedure->model()->removeRows(0,ui->tableWidget_lista_procedure->rowCount());
    //pulizia tabella - tipo 2
    //    ui->tableWidget_lista_procedure->clear();
    //    ui->tableWidget_lista_procedure->setRowCount(0);
    for (int row = 0; row < procedureVoto.size();row++){
        ui->tableWidget_lista_procedure->insertRow(ui->tableWidget_lista_procedure->rowCount());
        int rigaAggiunta = ui->tableWidget_lista_procedure->rowCount()-1;

        QTableWidgetItem *checkBoxItem = new QTableWidgetItem();
        checkBoxItem->setToolTip("seleziona procedura della riga corrispondente");
        checkBoxItem->setCheckState(Qt::Unchecked);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,0,checkBoxItem);

        uint idProcedura = procedureVoto.at(row).getIdProceduraVoto();
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(idProcedura));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,1,item);

        QString descrizione = QString::fromStdString(procedureVoto.at(row).getDescrizione());
        item = new QTableWidgetItem(descrizione);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,2,item);

        uint idRP = procedureVoto.at(row).getIdRP();
        item = new QTableWidgetItem(QString::number(idRP));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,3,item);

        QString qsInizio = QString::fromStdString(procedureVoto.at(row).getData_ora_inizio());
        item = new QTableWidgetItem(qsInizio);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,4,item);

        QString qsTermine = QString::fromStdString(procedureVoto.at(row).getData_ora_termine());
        item = new QTableWidgetItem(qsTermine);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,5,item);

        uint numSchede = procedureVoto.at(row).getNumSchedeVoto();
        item = new QTableWidgetItem(QString::number(numSchede));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,6,item);

        uint schedeInserite = procedureVoto.at(row).getSchedeInserite();
        item = new QTableWidgetItem(QString::number(schedeInserite));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,7,item);

        ProceduraVoto::statiProcedura statoProcedura = procedureVoto.at(row).getStato();
        QString stato = QString::fromStdString(ProceduraVoto::getStatoAsString(statoProcedura));
        item = new QTableWidgetItem(stato);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,8,item);

    }
    ui->tableWidget_lista_procedure->resizeColumnsToContents();
    ui->tableWidget_lista_procedure->horizontalHeader()->setStretchLastSection(true);

    idProceduraSelezionata = -1;
    statoProceduraSelezionata = ProceduraVoto::statiProcedura::undefined;
    ui->widget_azioni_procedura->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaProcedure);
}
void MainWindowTecnico::showViewSessioniProcedura(QList <SessioneVoto> sessioni){


    ui->label_descProc_s_content->setText(descProceduraSelezionata);
    ui->label_idProcedura_s_content->setText(QString::number(idProceduraSelezionata));

    ui->tableWidget_sessioni->model()->removeRows(0,ui->tableWidget_sessioni->rowCount());
    for (int row = 0; row < sessioni.size();row++){
        ui->tableWidget_sessioni->insertRow(ui->tableWidget_sessioni->rowCount());
        int rigaAggiunta = ui->tableWidget_sessioni->rowCount()-1;

        uint idSessione = sessioni.at(row).getIdSessione();
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(idSessione));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_sessioni->setItem(rigaAggiunta,0,item);

        string data = sessioni.at(row).getData();
        item = new QTableWidgetItem(QString::fromStdString(data));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_sessioni->setItem(rigaAggiunta,1,item);

        string apertura = sessioni.at(row).getOraApertura();
        item = new QTableWidgetItem(QString::fromStdString(apertura));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_sessioni->setItem(rigaAggiunta,2,item);

        string chiusura = sessioni.at(row).getOraChiusura();
        item = new QTableWidgetItem(QString::fromStdString(chiusura));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(Qt::NoItemFlags);
        item->setTextColor(Qt::black);
        ui->tableWidget_sessioni->setItem(rigaAggiunta,3,item);
    }

    ui->tableWidget_sessioni->resizeColumnsToContents();
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaSessioni);
}

void MainWindowTecnico::showViewSchedeProcedura(QList<SchedaVoto> schede)
{
    schedeOttenute = schede;

    //mostra la prima scheda
    mostraScheda();
}
void MainWindowTecnico::mostraScheda(){
    numSchede = schedeOttenute.size();
    if(numSchede == 0){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Nessuna scheda inserita, nulla da mostrare.");
        msgBox.exec();

    }
    if(numSchede == 1){
        ui->pushButton_successiva->setEnabled(false);
        ui->pushButton_precedente->setEnabled(false);
    }
    else if(numeroSchedaDaMostrare == 0){
        ui->pushButton_successiva->setEnabled(true);
    }

    ui->listWidget_candidati->clear();

    QFont serifFont("Times", 20, QFont::Bold);
    SchedaVoto schedaCorrente = schedeOttenute.at(numeroSchedaDaMostrare);
    uint codProcedura = schedaCorrente.getIdProceduraVoto();
    QListWidgetItem * item = new QListWidgetItem("id Procedura: " +
                                                 QString::number(codProcedura),ui->listWidget_candidati);
    item->setFont(serifFont);
    string descScheda = schedaCorrente.getDescrizioneElezione();
    item = new QListWidgetItem("Descrizione scheda: " +
                               QString::fromStdString(descScheda),ui->listWidget_candidati);

    item->setFont(serifFont);
    vector <uint> idTipiVotanti = schedaCorrente.getIdTipiVotantiConsentiti();
    QString listTipiVotanti;
    for (uint i = 0; i< idTipiVotanti.size(); i++){
        listTipiVotanti = listTipiVotanti + QString::number(idTipiVotanti.at(i));
        if(i+1 < idTipiVotanti.size() ){
            listTipiVotanti = listTipiVotanti + ", ";
        }
    }
    item = new QListWidgetItem("Id Tipi Votanti: " + listTipiVotanti,ui->listWidget_candidati);

    item->setFont(serifFont);
    uint numeroPreferenze = schedaCorrente.getNumPreferenze();
    item = new QListWidgetItem("Numero preferenze: " +
                               QString::number(numeroPreferenze),ui->listWidget_candidati);
    item->setFont(serifFont);

    vector <ListaElettorale> liste = schedaCorrente.getListeElettorali();
    for (uint listaIndex = 0; listaIndex < liste.size() ; listaIndex++){
        ListaElettorale listaCorrente = liste.at(listaIndex);
        vector <Candidato> candidatiLista = listaCorrente.getCandidati();

        //inserisco nome della lista
        string nomeLista = listaCorrente.getNome();
        if(nomeLista!="nessuna lista"){ //"nessuna lista" è il nome di default dato alla lista, quando il nome della lista non è richiesto per una votazione
            QString infoLista = "Lista " + QString::number(listaCorrente.getNumeroLista()) + ": " + QString::fromStdString(nomeLista);
            QListWidgetItem * item = new QListWidgetItem(infoLista,ui->listWidget_candidati);
            item->setFont(serifFont);
        }
        for (uint i = 0; i < candidatiLista.size(); i++ ){

            QString nominativo = QString::fromStdString(candidatiLista.at(i).getNome() + " "+ candidatiLista.at(i).getCognome());

            string matricola = candidatiLista.at(i).getMatricola();


            QListWidgetItem* item = new QListWidgetItem(nominativo,ui->listWidget_candidati);
            //item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
            //item->setCheckState(Qt::Unchecked);
            QVariant matrVariant(QString::fromStdString(matricola));
            item->setData(Qt::UserRole,matrVariant);

            string luogo = candidatiLista.at(i).getLuogoNascita();
            string data = candidatiLista.at(i).getDataNascita();
            QString infoCandidato = QString::fromStdString("nato a " + luogo + " il " + data);
            QVariant infoCandVariant(infoCandidato);
            item->setData(Qt::ToolTipRole,infoCandVariant);
        }


    }


    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaSchede);
}

void MainWindowTecnico::on_listWidget_candidati_itemChanged(QListWidgetItem *item)
{
    bool checked = item->checkState();
    if(checked){
        QVariant data = item->data(Qt::UserRole);
        QString matricola = data.toString();
        cout << "selezionato candidato con matricola: " << matricola.toStdString() << endl;
    }
}

//void MainWindowSeggio::on_aggiungiHT_button_clicked()
//{
//    QString codHT = ui->codHT_lineEdit->text();
//    ui->codHT_lineEdit->setText("");
//    ui->token_tableWidget->insertRow(ui->token_tableWidget->rowCount());
//    int rigaAggiunta = ui->token_tableWidget->rowCount()-1;
//    ui->token_tableWidget->setItem(rigaAggiunta,0,new QTableWidgetItem(codHT));
//    QString elimina = "Elimina";
//    ui->token_tableWidget->setItem(rigaAggiunta,1,new QTableWidgetItem(elimina));
//}
void MainWindowTecnico::on_pushButton_logout_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
}

void MainWindowTecnico::on_pushButton_crea_seggio_clicked()
{
    emit needInfoSeggi();

}

void MainWindowTecnico::on_pushButton_annulla_procedura_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    delete nuovaProcedura;
    pulisciInterfacciaCreazioneProcedura();

}
void MainWindowTecnico::pulisciInterfacciaCreazioneProcedura(){
    ui->comboBox_sessioni_inserite->clear();
    intervalliSessioni.clear();
    ui->dateEdit_data_sessione->clear();
    ui->lineEdit_descrizione_procedura->clear();
    ui->spinBox_numero_schede->setValue(1);
    ui->dateTimeEdit_data_ora_inizio->setDate(QDateTime::currentDateTime().date().addDays(1));
    ui->dateTimeEdit_data_ora_termine->setDate(QDateTime::currentDateTime().date().addDays(1));
    ui->timeEdit_apertura_sessione->clear();
    ui->timeEdit_chiusura_sessione->clear();
    ui->comboBox_idRP->clear();
}

void MainWindowTecnico::on_pushButton_salva_procedura_clicked()
{
    //TODO estrazione dati dalla schermata: descrizione, numero schede, identificativo RP, sessioni
    QString descrizione = ui->lineEdit_descrizione_procedura->text();
    nuovaProcedura->setDescrizione(descrizione.toStdString());

    int numSchede = ui->spinBox_numero_schede->value();
    nuovaProcedura->setNumSchedeVoto(numSchede);

    if(nuovaProcedura->getData_ora_inizio()==""){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Inserire almeno una procedura e una sessione prima di continuare.");
        msgBox.exec();
        ui->pushButton_salva_procedura->setEnabled(false);
        return;
    }
    if(intervalliSessioni.empty()){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Inserire almeno una sessione di voto.");
        msgBox.exec();
        ui->pushButton_salva_procedura->setEnabled(false);
        return;
    }

    if (ui->comboBox_idRP->currentText()==""){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Selezionare un Responsabile di Procedimento,se non è disponibile alcun Responsabile Procedimento si prega di registrarne almeno uno tramite la funzionalità apposita nella schermata precedente.");
        msgBox.exec();
        return;
    }
    else{
        QMessageBox msgBox(this);
        int index = ui->comboBox_idRP->currentIndex();
        uint idRP = nuovaProcedura->getRps().at(index).getIdRP();
        QString infoRP = QString::fromStdString(nuovaProcedura->getInfoRP(idRP));

        msgBox.setInformativeText("Sta per essere creata la procedura: "+ descrizione + ". Il Responsabile di procedimento sarà: " + infoRP);
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        //msgBox.buttons().at(0)->setText("Prosegui");
        msgBox.buttons().at(1)->setText("Annulla");
        int ret = msgBox.exec();
        if(ret == QMessageBox::Abort){
            return;
        }
        else{
            nuovaProcedura->setIdRP(idRP);
        }
    }


    //emettere segnale che la procedura è pronta per la memorizzazione sul db
    emit proceduraPronta(nuovaProcedura);
}

void MainWindowTecnico::on_pushButton_back_scelta_op_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
}

void MainWindowTecnico::on_pushButton_annulla_scheda_clicked()
{

    cout << "annullamento operazione: cancello la nuova scheda di voto" << endl;
    delete nuovaScheda;
    pulisciInterfacciaCreazioneScheda();

    on_pushButton_visualizza_procedure_clicked();
}

void MainWindowTecnico::pulisciInterfacciaCreazioneScheda(){
    hideBoxAggiungi();
    ui->comboBox_seleziona_candidato->clear();
    ui->comboBox_seleziona_lista_1->clear();
    ui->comboBox_seleziona_lista_2->clear();
    //ui->comboBox_tipo_elezione->clear();
    ui->spinBox_numero_preferenze->setMaximum(1);
    ui->spinBox_numero_preferenze->setMaximum(99);


}

void MainWindowTecnico::on_pushButton_addSchedaVoto_clicked()
{
    QDate resetDate = QDate::currentDate().addYears(-18);
    ui->dateEdit_data_nascita_c->setDate(resetDate);
    ui->dateEdit_data_nascita_c->setMaximumDate(resetDate);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Creazione nuova scheda");
    msgBox.setInformativeText("Stai per inserire una scheda di voto per la procedura con id: " + QString::number(idProceduraSelezionata));
    //msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.buttons().at(0)->setText("Procedi");
    msgBox.buttons().at(1)->setText("Annulla");
    int ret = msgBox.exec();

    if (ret==QMessageBox::Cancel){
        return;
    }

    ui->formWidget_candidato->hide();
    ui->formWidget_lista->hide();
    ui->pushButton_annulla_aggiungi->hide();
    ui->pushButton_conferma_aggiungi->hide();
    ui->pushButton_aggiungi_lista->setEnabled(true);



    //    ui->comboBox_tipo_elezione->clear();
    //    QStringList strList;
    //    strList << "Senato Accademico" << "Consiglio di Amministrazione" << "Rappresentanti del Personale T.A." << "Consiglio di Corso di Studio" << "Consiglio di Interclasse" << "Consiglio di Dipartimento" ;
    //    ui->comboBox_tipo_elezione->addItems(strList);

    emit needTipiVotanti();

}

void MainWindowTecnico::createScheda(vector<TipoVotante> tipiVotanti){

    //disconnettiamo temporaneamente l'evento che controlla il numero di tipo votanti selezionati
    QObject::disconnect(ui->listWidget_tipoVotanti,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(on_listWidget_tipoVotanti_itemChanged(QListWidgetItem*)));
    //svuotiamo il listWidget_tipoVotanti da eventuali dati precedenti
    ui->listWidget_tipoVotanti->clear();
    for (uint i = 0; i < tipiVotanti.size(); i++ ){

        QString descrizione = QString::fromStdString(tipiVotanti.at(i).getDescrizione());

        uint id = tipiVotanti.at(i).getId();

        QListWidgetItem* item = new QListWidgetItem(descrizione);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        //item->setFlags(item->flags() & !Qt::ItemIsEditable);
        item->setCheckState(Qt::Unchecked);
        QVariant idVariant(QString::number(id));
        item->setData(Qt::UserRole,idVariant);
        ui->listWidget_tipoVotanti->addItem(item);

    }

    this->numTipoVotantiChecked = 0;

    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::creazioneSchede);

    nuovaScheda = new SchedaVoto();
    nuovaScheda->setIdProceduraVoto(this->idProceduraSelezionata);
    QObject::connect(ui->listWidget_tipoVotanti,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(on_listWidget_tipoVotanti_itemChanged(QListWidgetItem*)));

}

void MainWindowTecnico::on_pushButton_aggiungi_candidato_clicked()
{
    ui->pushButton_completa_scheda->setEnabled(false);
    ui->pushButton_rimuovi_candidato->setEnabled(false);
    ui->pushButton_rimuovi_gruppo->setEnabled(false);
    ui->pushButton_aggiungi_lista->setEnabled(false);
    ui->pushButton_aggiungi_candidato->setEnabled(false);
    ui->pushButton_conferma_aggiungi->setEnabled(true);

    nuovaScheda->setModalitaAdd(SchedaVoto::modoAdd::candidato);

    ui->comboBox_seleziona_lista_1->clear();


    vector <ListaElettorale> listeElettorali = nuovaScheda->getListeElettorali();
    for(unsigned i=0; i< listeElettorali.size(); ++i){
        QString str = QString::fromStdString(listeElettorali.at(i).getNome());
        ui->comboBox_seleziona_lista_1->addItem(str);

    }



    //mostra area aggiunzione candidato
    ui->formWidget_candidato->show();
    ui->pushButton_annulla_aggiungi->show();
    ui->pushButton_conferma_aggiungi->show();

}

void MainWindowTecnico::on_pushButton_aggiungi_lista_clicked()
{
    ui->pushButton_completa_scheda->setEnabled(false);
    ui->pushButton_rimuovi_candidato->setEnabled(false);
    ui->pushButton_rimuovi_gruppo->setEnabled(false);
    ui->pushButton_aggiungi_lista->setEnabled(false);
    ui->pushButton_aggiungi_candidato->setEnabled(false);
    ui->pushButton_conferma_aggiungi->setEnabled(false);

    nuovaScheda->setModalitaAdd(SchedaVoto::modoAdd::lista);
    //mostra area aggiunzione associazione_gruppo
    ui->formWidget_lista->show();
    ui->pushButton_conferma_aggiungi->show();
    ui->pushButton_annulla_aggiungi->show();


}

void MainWindowTecnico::on_pushButton_conferma_aggiungi_clicked()
{
    uint mod = nuovaScheda->getModalitaAdd();

    if(mod == SchedaVoto::modoAdd::candidato){
        QString matricola = ui->lineEdit_matricola->text();
        string strMatricola = matricola.toStdString();

        QString nome = ui->lineEdit_nome_c->text();
        string strNome = nome.toStdString();
        QString cognome = ui->lineEdit_cognome_c->text();
        string strCognome = cognome.toStdString();
        QString luogoNascita = ui->lineEdit_luogo_nascita_c->text();
        string strLuogoNascita = luogoNascita.toStdString();

        if(strNome =="" || strCognome == "" || strLuogoNascita =="" || strMatricola==""){
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Attenzione");
            msgBox.setInformativeText("Compilare tutti i campi del candidato");
            msgBox.exec();
            return;
        }




        QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
        if (!(re.exactMatch(matricola))){
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Errore");
            msgBox.setInformativeText("Matricola inserita non valida. La matricola deve essere numerica");
            msgBox.exec();
            return;
        }

        QString lista = ui->comboBox_seleziona_lista_1->currentText();
        string strLista = lista.toStdString();
        if(strLista == ""){
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Vuoi inserire un candidato senza lista? Il candidato verrà aggiunto alla lista <nessuna lista> e non sarà possibile aggiungere delle liste");
            //msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
            msgBox.buttons().at(0)->setText("Si");
            msgBox.buttons().at(1)->setText("No");
            //msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            if (ret==QMessageBox::Cancel){
                //si è scelto di non aggiungere il candidato senza una lista, non si conferma l'aggiunzione del candidato
                return;
            }
            else{
                strLista = "nessuna lista";
                ui->comboBox_seleziona_lista_2->addItem(QString::fromStdString(strLista));
                ui->pushButton_aggiungi_lista->setEnabled(false);
                nuovaScheda->soloCandidatiOn();
            }
        }


        QDate dataNascita = ui->dateEdit_data_nascita_c->date();
        string strDataNascita = dataNascita.toString("dd/MM/yyyy").toStdString();


        if(nuovaScheda->addCandidato(strMatricola,strNome,strCognome,strLista,strDataNascita,strLuogoNascita)){
            QString itemCandidato = matricola + ", " + nome + " " + cognome;
            if(strLista!="nessuna lista"){
                itemCandidato = itemCandidato +  " - " + QString::fromStdString(strLista);
            }
            ui->comboBox_seleziona_candidato->addItem(itemCandidato);
            hideBoxAggiungi();
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Il candidato con matricola " + matricola + " è stato aggiunto correttamente alla lista: " + QString::fromStdString(strLista) );
            msgBox.exec();
        }
        else{
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Un candidato con matricola " + matricola + " è già presente, impossibile riaggiungerlo o aggiungerne un altro con la stessa matricola, verificare se il numero di matricola inserito è corretto!");
            msgBox.exec();
        }
    }
    else{
        //modo: aggiungi lista
        QString lista = ui->lineEdit_nuova_lista->text();
        string strLista = lista.toStdString();
        nuovaScheda->addLista(strLista);
        ui->comboBox_seleziona_lista_2->addItem(lista);
        hideBoxAggiungi();
        ui->pushButton_aggiungi_lista->setEnabled(true);
    }

}

void MainWindowTecnico::on_pushButton_completa_scheda_clicked()
{

    if(nuovaScheda->getCandidati().size()<2){
        //se non sono stati inseriti almeno due candidati alla scheda di voto
        QMessageBox::information(this,"Error Message","Inserire almeno due candidati prima di completare la scheda");
        return;
    }

    if(ui->lineEdit_descrizioneQuesitoScheda->text()==""){
        QMessageBox::information(this,"Error Message","Il campo descrizione scheda è obbligatorio");
        return;

    }
    else{
        nuovaScheda->setDescrizioneElezione(ui->lineEdit_descrizioneQuesitoScheda->text().toStdString());
    }

    if(this->numTipoVotantiChecked ==0){
        QMessageBox::information(this,"Error Message","Selezionare almeno un tipo di votante per la scheda corrente");
        return;
    }
    else{
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Completare la creazione della scheda?");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.buttons().at(0)->setText("Conferma");
        msgBox.buttons().at(1)->setText("Rivedi");
        int ret = msgBox.exec();
        if(ret == QMessageBox::Cancel){
            return;
        }
        //aggiungo alla scheda di voto gli id dei tipi di votanti che possono compilare questa scheda
        for(int i = 0; i < ui->listWidget_tipoVotanti->count(); ++i)
        {
            QListWidgetItem* item = ui->listWidget_tipoVotanti->item(i);
            if(item->checkState()==Qt::CheckState::Checked){
                QVariant var = item->data(Qt::UserRole);
                nuovaScheda->addIdTipiVotantiConsentiti(var.toUInt());
            }
        }

        uint numPref = ui->spinBox_numero_preferenze->text().toUInt();
        if(numPref == 0){
            numPref = 1;
        }
        nuovaScheda->setNumPreferenze(numPref);


        emit schedaPronta(nuovaScheda);

        cout << "emesso il segnale di scheda pronta" << endl;
    }

}

void MainWindowTecnico::on_pushButton_rimuovi_candidato_clicked()
{
    if(ui->comboBox_seleziona_candidato->currentText()!=""){
        int index = ui->comboBox_seleziona_candidato->currentIndex();
        nuovaScheda->removeCandidatoFromLista(index);
        ui->comboBox_seleziona_candidato->removeItem(index);

        if(ui->comboBox_seleziona_candidato->count()==0){
            if(ui->comboBox_seleziona_lista_2->currentText()=="nessuna lista"){
                //dobbiamo anche eliminare la procedura fittizzia

                vector <ListaElettorale> listElettorali = nuovaScheda->getListeElettorali();
                uint index;
                //troviamo l'indidce della lista di rimuovere
                QString entry = ui->comboBox_seleziona_lista_2->currentText();
                for (uint i = 0; i < listElettorali.size(); i++){
                    if(listElettorali.at(i).getNome() == entry.toStdString()){
                        index = i;
                        break;
                    }
                }
                vector <Candidato> candidatiDaRimuovere =  nuovaScheda->removeLista(index);
                nuovaScheda->removeCandidatiFromScheda(candidatiDaRimuovere);

                ui->comboBox_seleziona_lista_2->removeItem(index);
                if(ui->comboBox_seleziona_lista_2->count() == 0){
                    //era stato disabilitato nel caso di creazione scheda senza liste
                    nuovaScheda->soloCandidatiOff();
                    ui->pushButton_aggiungi_lista->setEnabled(true);
                }
            }
        }

        //se
        if(ui->comboBox_seleziona_lista_2->count()==0){
            nuovaScheda->soloCandidatiOff();
            ui->pushButton_aggiungi_lista->setEnabled(true);
        }
    }
    else{
        QMessageBox msb(this);
        msb.setText("Nulla da eliminare");
        msb.exec();
    }
}

void MainWindowTecnico::on_pushButton_rimuovi_gruppo_clicked()
{
    if(ui->comboBox_seleziona_lista_2->currentText()==""){
        QMessageBox msb(this);
        msb.setText("Nulla da eliminare");
        msb.exec();
    }
    else{
        QString entry = ui->comboBox_seleziona_lista_2->currentText();
        //informiamo l'utente dell'effettivo effetto dell'operazione, e chiediamo conferma prima di proseguire
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Attenzione!");
        msgBox.setInformativeText("Stai eliminando la lista: " + entry +". Se rimuovi la lista, verranno eliminati tutti i candidati associati ad essa. Vuoi proseguire?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.buttons().at(0)->setText("Si");
        msgBox.buttons().at(1)->setText("No");
        //msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        //se è stato cliccato il tasto cancel, interrompiamo l'operazione
        if (ret==QMessageBox::Cancel){
            return;
        }


        vector <ListaElettorale> listElettorali = nuovaScheda->getListeElettorali();
        uint index;

        //troviamo l'indice della lista da rimuovere
        for (uint i = 0; i < listElettorali.size(); i++){
            if(listElettorali.at(i).getNome() == entry.toStdString()){
                index = i;
                break;
            }
        }

        vector <Candidato> candidatiDaRimuovere =  nuovaScheda->removeLista(index);
        if(nuovaScheda->getListeElettorali().size() == 0){
            nuovaScheda->soloCandidatiOff();
        }
        nuovaScheda->removeCandidatiFromScheda(candidatiDaRimuovere);


        ui->comboBox_seleziona_lista_2->removeItem(index);
        if(ui->comboBox_seleziona_lista_2->count() == 0){
            //era stato disabilitato nel caso di creazione scheda senza liste
            ui->pushButton_aggiungi_lista->setEnabled(true);
        }


        //aggiornamento comboBoxListaCandidati
        ui->comboBox_seleziona_candidato->clear();
        vector <Candidato> candidati = nuovaScheda->getCandidati();
        for(unsigned i=0; i< candidati.size(); ++i){
            QString nome = QString::fromStdString(candidati.at(i).getNome());
            QString cognome = QString::fromStdString(candidati.at(i).getCognome());
            QString matricola = QString::fromStdString(candidati.at(i).getMatricola());
            ui->comboBox_seleziona_candidato->addItem(matricola + ", " + nome + " " + cognome);
        }

    }
}

void MainWindowTecnico::on_pushButton_annulla_aggiungi_clicked()
{
    hideBoxAggiungi();
    if(nuovaScheda->getModalitaAdd()==SchedaVoto::modoAdd::lista){
        ui->pushButton_aggiungi_lista->setEnabled(true);
    }
}

void MainWindowTecnico::hideBoxAggiungi(){
    ui->formWidget_lista->hide();
    ui->lineEdit_nuova_lista->clear();
    ui->lineEdit_matricola->clear();
    ui->formWidget_candidato->hide();
    ui->lineEdit_nome_c->clear();
    ui->lineEdit_cognome_c->clear();
    QDate resetDate = QDate::currentDate().addYears(-18);
    ui->dateEdit_data_nascita_c->setDate(resetDate);
    ui->dateEdit_data_nascita_c->setMaximumDate(resetDate);
    ui->lineEdit_luogo_nascita_c->clear();
    ui->pushButton_annulla_aggiungi->hide();
    ui->pushButton_conferma_aggiungi->hide();


    ui->pushButton_completa_scheda->setEnabled(true);
    ui->pushButton_rimuovi_candidato->setEnabled(true);
    ui->pushButton_rimuovi_gruppo->setEnabled(true);

    ui->pushButton_aggiungi_candidato->setEnabled(true);

    if(ui->comboBox_seleziona_lista_2->currentText()=="nessuna lista"){
        ui->pushButton_aggiungi_lista->setEnabled(false);
    }
    else{
        ui->pushButton_aggiungi_lista->setEnabled(false);
    }

    if(!nuovaScheda->soloCandidatiMode()){
        ui->pushButton_aggiungi_lista->setEnabled(true);
    }
}


void MainWindowTecnico::on_lineEdit_nuova_lista_textChanged(const QString &arg1)
{
    if(arg1==""){
        ui->pushButton_conferma_aggiungi->setEnabled(false);
    }
    else{
        ui->pushButton_conferma_aggiungi->setEnabled(true);

    }
}

void MainWindowTecnico::on_lineEdit_descrizione_procedura_textChanged(const QString &arg1)
{
    if(arg1==""){
        ui->pushButton_salva_procedura->setEnabled(false);
    }
    else{
        ui->pushButton_salva_procedura->setEnabled(true);

    }
}

void MainWindowTecnico::on_pushButton_memorizza_periodo_procedura_clicked()
{
    QDateTime inizio = ui->dateTimeEdit_data_ora_inizio->dateTime();
    QDateTime termine = ui->dateTimeEdit_data_ora_termine->dateTime();

    if(inizio < termine){
        bool periodoMaiRegistratoPrima = (nuovaProcedura->getData_ora_inizio()=="");
        //cout << periodoMaiRegistratoPrima << endl;
        if( !periodoMaiRegistratoPrima &&
                (nuovaProcedura->getData_ora_inizio()!=inizio.toString("yyyy/MM/dd hh:mm").toStdString() ||
                 nuovaProcedura->getData_ora_termine()!=termine.toString("yyyy/MM/dd hh:mm").toStdString())){
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Modificare il periodo della procedura? Le sessioni già inserite verranno rimosse ");
            msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
            msgBox.buttons().at(0)->setText("Conferma");
            msgBox.buttons().at(1)->setText("Annulla");
            int ret = msgBox.exec();
            if(ret == QMessageBox::Cancel){
                return;
            }
        }

        //TODO conferma che per questo range di date non siano già presenti delle procedure
        emit checkRangeProcedura(inizio,termine);


    }
    else{
        QMessageBox msgBox(this);
        msgBox.setInformativeText("La data di inizio procedura deve precedere quella di termine procedura.");
        msgBox.exec();
    }
}

void MainWindowTecnico::setPeriodoProcedura(QDateTime inizio, QDateTime termine){
    //elimina eventuali sessioni memorizzate relative a precedenti periodi di procedura
    nuovaProcedura->resetSessioni();
    intervalliSessioni.clear();
    ui->comboBox_sessioni_inserite->clear();
    ui->pushButton_elimina_sessione->setEnabled(false);

    //memorizza periodo procedura
    string strInizio= inizio.toString("yyyy/MM/dd hh:mm").toStdString();
    //cout << strInizio << endl;
    nuovaProcedura->setData_ora_inizio(strInizio);
    string strTermine= termine.toString("yyyy/MM/dd hh:mm").toStdString();
    nuovaProcedura->setData_ora_termine(strTermine);

    ui->dateEdit_data_sessione->setMinimumDate(inizio.date());
    ui->dateEdit_data_sessione->setMaximumDate(termine.date());
    ui->pushButton_aggiungi_sessione->setEnabled(true);
    ui->dateEdit_data_sessione->setEnabled(true);
    ui->timeEdit_apertura_sessione->setTime(inizio.time());
    ui->timeEdit_apertura_sessione->setEnabled(true);
    ui->timeEdit_chiusura_sessione->setEnabled(true);
}

void MainWindowTecnico::messageProceduraRangeInUse()
{
    QMessageBox::information(this,"Errore","Il range selezionato è sovrapposto con quello di una o più procedure presenti nel sistema");

}

void MainWindowTecnico::showViewSeggi(vector<InfoSeggio> seggiPresenti)
{
    ui->lineEdit_denominazioneIndirizzo->clear();
    ui->comboBox_selezionaSeggioDaRimuovere->clear();
    QString item;
    seggiOttenuti = seggiPresenti;
    for(uint i = 0; i < seggiPresenti.size(); i++){
        string desc = seggiPresenti.at(i).getDescrizione();
        string ip = seggiPresenti.at(i).getIp();
        item = QString::fromStdString(desc) + ", IP: " + QString::fromStdString(ip);
        ui->comboBox_selezionaSeggioDaRimuovere->addItem(item);
    }

    int byte1, byte2, byte3, byte4;
    char dot;
    string subnetSeggi = "192.168.56.0";
    istringstream s(subnetSeggi);  // input stream that now contains the ip address string

    s >> byte1 >> dot >> byte2 >> dot >> byte3 >> dot >> byte4 >> dot;

    //calcolo di tutti i possibili ip  di Seggio nella sotto rete
    vector <string> ipSeggiDisponibili;
    for (uint i = 4; i <=252 ; i=i+4){
        string ipSeggio = to_string(byte1) + "." + to_string(byte2) + "." + to_string(byte3) + "." +to_string(i);
        ipSeggiDisponibili.push_back(ipSeggio);
    }

    //esclusione degli ip già impegnati da altri seggi
    for(uint i = 0; i < seggiPresenti.size(); i++){
        string ip = seggiPresenti.at(i).getIp();
        for(uint j = 0; j < ipSeggiDisponibili.size();j++){
            if(ip == ipSeggiDisponibili.at(j)){
                cout << "trovato ipSeggio già presente, lo elimino dai disponibili" << endl;
                ipSeggiDisponibili.erase(ipSeggiDisponibili.begin()+j);
                break;
            }
        }
    }

    //riempio il combo box degli ip di seggio disponibili
    ui->comboBox_ipSeggioDisponibili->clear();
    for (uint i = 0; i < ipSeggiDisponibili.size(); i++){
        ui->comboBox_ipSeggioDisponibili->addItem(
                    QString::fromStdString(
                        ipSeggiDisponibili.at(i)));
    }




    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::creazioneSeggio);
}

void MainWindowTecnico::addGeneratoriOTP(uint idSeggio){
    initTableHT();
    idNuovoSeggio = idSeggio;
    numHTaggiunti = 0;
    ui->pushButton_completaCreazioneSeggio->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::aggiuntaTokenSeggio);

}

void MainWindowTecnico::showMessaggeSeggioCreato()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Success");
    msgBox.setInformativeText("Creazione del seggio completata.");
    msgBox.exec();

    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
}

void MainWindowTecnico::showMessageCreazioneSeggioAnnullata()
{
    QMessageBox msgBox(this);
    msgBox.setInformativeText("Creazione seggio annullata");
    msgBox.exec();
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    return;
}

void MainWindowTecnico::addTokenToTable(string sn, string user, string pass, uint idSeggio){
    uint indexNewRow = ui->tableWidget_lista_procedure->rowCount();
    cout << "indice nuova riga: " << indexNewRow << endl;
     ui->tableWidget_lista_procedure->insertRow(indexNewRow);
    ui->tableWidget_hardwareToken->insertRow(indexNewRow);
    int rigaAggiunta = ui->tableWidget_hardwareToken->rowCount()-1;


    QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(sn));
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::NoItemFlags);
    item->setTextColor(Qt::black);
    ui->tableWidget_hardwareToken->setItem(rigaAggiunta,0,item);

    QString username = QString::fromStdString(user);
    item = new QTableWidgetItem(username);
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::NoItemFlags);
    item->setTextColor(Qt::black);
    ui->tableWidget_hardwareToken->setItem(rigaAggiunta,1,item);

    item = new QTableWidgetItem(QString::fromStdString(pass));
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::NoItemFlags);
    item->setTextColor(Qt::black);
    ui->tableWidget_hardwareToken->setItem(rigaAggiunta,2,item);

    item = new QTableWidgetItem(QString::number(idSeggio));
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::NoItemFlags);
    item->setTextColor(Qt::black);
    ui->tableWidget_hardwareToken->setItem(rigaAggiunta,3,item);


    numHTaggiunti++;
    ui->formWidget_testHT->clearMask();
    if(numHTaggiunti == 5){
        ui->pushButton_testOTP->setEnabled(false);
        ui->formWidget_testHT->setEnabled(false);
        ui->pushButton_completaCreazioneSeggio->setEnabled(true);
    }
}

void MainWindowTecnico::showMessageTokenNotAvailable()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Errore");
    msgBox.setInformativeText("Token non disponibile, è già stato associato ad un altro seggio.");
    msgBox.exec();
    return;
}

void MainWindowTecnico::showMessageTestTokenFail()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Errore");
    msgBox.setInformativeText("Test del token fallito. Ricontrollare i dati inseriti e riprovare.");
    msgBox.exec();
    return;
}

void MainWindowTecnico::on_pushButton_aggiungi_sessione_clicked()
{
    QDate dataSessione = ui->dateEdit_data_sessione->date();
    QTime oraApertura = ui->timeEdit_apertura_sessione->time();
    QTime oraChiusura = ui->timeEdit_chiusura_sessione->time();


    QDateTime data_ora_InizioProcedura = QDateTime::fromString(QString::fromStdString(nuovaProcedura->getData_ora_inizio()),"yyyy/MM/dd hh:mm");
    QDateTime data_ora_TermineProcedura = QDateTime::fromString(QString::fromStdString(nuovaProcedura->getData_ora_termine()),"yyyy/MM/dd hh:mm");
    QDate dataInizioProcedura = data_ora_InizioProcedura.date();
    QDate dataTermineProcedura = data_ora_TermineProcedura.date();
    QTime oraInizioProcedura = data_ora_InizioProcedura.time();
    QTime oraTermineProcedura = data_ora_TermineProcedura.time();
    if(dataSessione == dataInizioProcedura){
        if(oraApertura < oraInizioProcedura){
            QMessageBox msgBox(this);
            msgBox.setInformativeText("L'ora di apertura sessione non può essere precedente all'ora di inizio procedura.");
            msgBox.exec();
            return;
        }
    }

    if(dataSessione == dataTermineProcedura){
        if(oraChiusura > oraTermineProcedura){
            QMessageBox msgBox(this);
            msgBox.setInformativeText("L'ora di chiusura sessione non può essere successiva all'ora di fine procedura.");
            msgBox.exec();
            return;
        }
    }

    if(oraApertura>=oraChiusura){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("L'ora di chiusura sessione deve essere successiva all'ora di apertura sessione.");
        msgBox.exec();
        return;
    }

    nuovaSessione = new SessioneVoto();
    nuovaSessione->setData(dataSessione.toString("yyyy/MM/dd").toStdString());
    nuovaSessione->setOraApertura(oraApertura.toString("hh:mm").toStdString());
    nuovaSessione->setOraChiusura(oraChiusura.toString("hh:mm").toStdString());

    QDateTime dtAperturaSessione = QDateTime(dataSessione,oraApertura);
    QDateTime dtChiusuraSessione = QDateTime(dataSessione,oraChiusura);


    SessioniQt s(dtAperturaSessione,dtChiusuraSessione);
    if(intervalliSessioni.empty()){
        //nel caso in cui sto inserendo la prima sessione, non sono necessari controlli di sovrapposizione
        nuovaProcedura->addSessione(nuovaSessione);
        intervalliSessioni.push_back(s);
        ui->comboBox_sessioni_inserite->addItem(dtAperturaSessione.toString("dd/MM/yyyy hh:mm") + " - " + dtChiusuraSessione.toString("dd/MM/yyyy hh:mm"));
        ui->pushButton_elimina_sessione->setEnabled(true);
        ui->pushButton_salva_procedura->setEnabled(true);
    }
    else{
        bool sovrapposizione = false;
        for(uint i = 0; i < intervalliSessioni.size(); i++){
            bool type1 = (dtAperturaSessione <= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione >= intervalliSessioni.at(i).getFine()); //
            bool type2 = (dtAperturaSessione >= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getFine()); //
            bool type3 = (dtChiusuraSessione >= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getFine());//
            bool type4 = (dtAperturaSessione >= intervalliSessioni.at(i).getInizio()) && (dtAperturaSessione <= intervalliSessioni.at(i).getFine());//
            if(type1 || type2 || type3 ||type4){
                cout << "sovrapposizioni sessioni rilevate" << endl;
                sovrapposizione = true;
                break;
            }
        }
        if (!sovrapposizione){
            nuovaProcedura->addSessione(nuovaSessione);
            intervalliSessioni.push_back(s);
            ui->comboBox_sessioni_inserite->addItem(dtAperturaSessione.toString("dd/MM/yyyy hh:mm") + " - " + dtChiusuraSessione.toString("dd/MM/yyyy hh:mm"));
            ui->pushButton_elimina_sessione->setEnabled(true);
            ui->pushButton_salva_procedura->setEnabled(true);
        }
        else{
            QMessageBox msgBox(this);
            msgBox.setInformativeText("La sessione da inserire è sovrapposta ad altre sessioni già inserite");
            msgBox.exec();
        }

    }
    delete nuovaSessione;
    return;
}

void MainWindowTecnico::on_dateTimeEdit_data_ora_inizio_dateTimeChanged(const QDateTime &dateTime)
{
    ui->dateTimeEdit_data_ora_termine->setMinimumDateTime(dateTime);
}

void MainWindowTecnico::on_timeEdit_apertura_sessione_timeChanged(const QTime &time)
{
    ui->timeEdit_chiusura_sessione->setMinimumTime(time);
}

void MainWindowTecnico::on_pushButton_elimina_sessione_clicked()
{
    int index = ui->comboBox_sessioni_inserite->currentIndex();
    ui->comboBox_sessioni_inserite->removeItem(index);
    intervalliSessioni.erase(intervalliSessioni.begin()+index);
    nuovaProcedura->removeSessioneByIndex(index);
    if(intervalliSessioni.empty()){
        ui->pushButton_elimina_sessione->setEnabled(false);
    }
}

void MainWindowTecnico::on_pushButton_annulla_rp_clicked()
{
    delete nuovoRP;
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    pulisciInterfacciaCreazioneRP();
}

void MainWindowTecnico::pulisciInterfacciaCreazioneRP(){
    ui->lineEdit_nome_rp->clear();
    ui->lineEdit_cognome_rp->clear();
    QDate maggiorenne = QDate::currentDate().addYears(-18);
    ui->dateEdit_data_nascita_rp->setDate(maggiorenne);
    ui->dateEdit_data_nascita_rp->setMaximumDate(maggiorenne);
    ui->lineEdit_luogo_nascita_rp->clear();
    ui->lineEdit_password_rp->clear();
    ui->lineEdit_ripeti_password_rp->clear();
}

void MainWindowTecnico::on_pushButton_completa_reg_rp_clicked()
{
    QString pass1,pass2;
    pass1 = ui->lineEdit_password_rp->text();
    pass2 = ui->lineEdit_ripeti_password_rp->text();
    if(pass1!=pass2){
        ui->label_error_password_rp->show();
        return;
    }

    QString nome = ui->lineEdit_nome_rp->text();
    QString cognome = ui->lineEdit_cognome_rp->text();
    QString luogoNascita = ui->lineEdit_luogo_nascita_rp->text();
    QDate dataNascita = ui->dateEdit_data_nascita_rp->date();
    nuovoRP->setNome(nome.toStdString());
    nuovoRP->setCognome(cognome.toStdString());
    nuovoRP->setLuogoNascita(luogoNascita.toStdString());
    nuovoRP->setPassword(pass1.toStdString());
    nuovoRP->setDataNascita(dataNascita.toString("yyyy/MM/dd").toStdString());

    //emetto il segnale di RP pronto alla memorizzazione
    emit rpPronto(nuovoRP);
}

void MainWindowTecnico::on_lineEdit_nome_rp_textChanged(const QString &arg1)
{
    if(arg1==""){
        ui->pushButton_completa_reg_rp->setEnabled(false);
    }
    else{
        QString cognome,luogo,pass1,pass2;
        cognome = ui->lineEdit_cognome_rp->text();
        luogo = ui->lineEdit_luogo_nascita_rp->text();
        pass1 = ui->lineEdit_password_rp->text();
        pass2 = ui->lineEdit_ripeti_password_rp->text();
        if(cognome!="" && luogo !="" && pass1!="" &&pass2!=""){
            ui->pushButton_completa_reg_rp->setEnabled(true);
        }
    }
}

void MainWindowTecnico::on_lineEdit_cognome_rp_textChanged(const QString &arg1)
{
    if(arg1==""){
        ui->pushButton_completa_reg_rp->setEnabled(false);
    }
    else{
        QString nome,luogo,pass1,pass2;
        nome = ui->lineEdit_nome_rp->text();
        luogo = ui->lineEdit_luogo_nascita_rp->text();
        pass1 = ui->lineEdit_password_rp->text();
        pass2 = ui->lineEdit_ripeti_password_rp->text();
        if(nome!="" && luogo !="" && pass1!="" &&pass2!=""){
            ui->pushButton_completa_reg_rp->setEnabled(true);
        }
    }
}

void MainWindowTecnico::on_lineEdit_password_rp_textChanged(const QString &arg1)
{
    ui->lineEdit_ripeti_password_rp->clear();
    ui->label_error_password_rp->hide();
    if(arg1==""){
        ui->pushButton_completa_reg_rp->setEnabled(false);
    }
}

void MainWindowTecnico::on_lineEdit_ripeti_password_rp_textChanged(const QString &arg1)
{
    if(arg1==""){
        ui->pushButton_completa_reg_rp->setEnabled(false);
    }
    else{
        QString nome,cognome,luogo,pass1,pass2;
        nome = ui->lineEdit_nome_rp->text();
        cognome = ui->lineEdit_cognome_rp->text();
        luogo = ui->lineEdit_luogo_nascita_rp->text();
        pass1 = ui->lineEdit_password_rp->text();
        pass2 = ui->lineEdit_ripeti_password_rp->text();
        if(nome!="" && cognome!="" && luogo !="" && pass1!="" &&pass2!=""){
            ui->pushButton_completa_reg_rp->setEnabled(true);
        }
    }
}

void MainWindowTecnico::on_pushButton_visualizzaInfoRP_clicked()
{
    if(ui->comboBox_idRP->currentText()!=""){
        uint index = ui->comboBox_idRP->currentIndex();
        vector <ResponsabileProcedimento> rps = nuovaProcedura->getRps();
        uint idRP = rps.at(index).getIdRP();
        QString infoRP = QString::fromStdString(nuovaProcedura->getInfoRP(idRP));

        QMessageBox::information(this,"InfoRP: " + QString::number(idRP),infoRP);
    }
}

uint MainWindowTecnico::getIdProceduraSelezionata() const
{
    return idProceduraSelezionata;
}

void MainWindowTecnico::setIdProceduraSelezionata(const uint &value)
{
    idProceduraSelezionata = value;
}

void MainWindowTecnico::on_tableWidget_lista_procedure_cellClicked(int row, int column)
{
    unsigned int currentRow = row;
    if(column==0){
        if(ui->tableWidget_lista_procedure->item(row,0)->checkState() == Qt::Checked){
            uint tempID = ui->tableWidget_lista_procedure->item(currentRow,1)->text().toUInt();
            if(tempID!=idProceduraSelezionata){
                idProceduraSelezionata = tempID;

                QString stato = ui->tableWidget_lista_procedure->item(currentRow,8)->text();
                statoProceduraSelezionata = ProceduraVoto::getStatoFromString(stato.toStdString());
                descProceduraSelezionata = ui->tableWidget_lista_procedure->item(currentRow,2)->text();
                uint numSchedeInserite = ui->tableWidget_lista_procedure->item(currentRow,7)->text().toUInt();
                cout << "id Procedura selezionata: " << idProceduraSelezionata << ", stato: " << ProceduraVoto::getStatoAsString(statoProceduraSelezionata) << endl;
                unsigned int numberRows = ui->tableWidget_lista_procedure->rowCount();

                //se abbiamo selezionato una riga diversa dalla precedente, deselezioniamo qualsiasi selezione precedente
                for (unsigned int rowIndex = 0; rowIndex < numberRows; rowIndex++){
                    if(rowIndex!=currentRow){
                        ui->tableWidget_lista_procedure->item(rowIndex,0)->setCheckState(Qt::Unchecked);
                    }
                }


                //            if(statoProceduraSelezionata != ){
                //                ui->pushButton_addSchedaVoto->setEnabled(false);
                //            }
                //            if(statoProceduraSelezionata=="in corso" || statoProceduraSelezionata == "conclusa" || statoProceduraSelezionata == "scrutinata"){
                //                ui->pushButton_removeProcedura->setEnabled(false);
                //            }
                //            else{
                //                ui->pushButton_addSchedaVoto->setEnabled(true);
                //            }

                if(numSchedeInserite==0){
                    ui->pushButton_visualizza_schede->setEnabled(false);
                }
                else{
                    ui->pushButton_visualizza_schede->setEnabled(true);
                }

                switch(statoProceduraSelezionata){
                case ProceduraVoto::statiProcedura::programmata:
                    ui->pushButton_removeProcedura->setEnabled(true);
                    ui->pushButton_addSchedaVoto->setEnabled(false);
                    ui->pushButton_visualizzaSessioni->setEnabled(true);
                    ui->pushButton_printSessionKeys->setEnabled(true);
                    break;
                case ProceduraVoto::statiProcedura::in_corso:
                    ui->pushButton_removeProcedura->setEnabled(false);
                    ui->pushButton_addSchedaVoto->setEnabled(false);
                    ui->pushButton_visualizzaSessioni->setEnabled(true);
                    ui->pushButton_printSessionKeys->setEnabled(true);
                    break;
                case ProceduraVoto::statiProcedura::conclusa:
                case ProceduraVoto::statiProcedura::scrutinata:
                    ui->pushButton_removeProcedura->setEnabled(false);
                    ui->pushButton_addSchedaVoto->setEnabled(false);
                    ui->pushButton_visualizzaSessioni->setEnabled(true);
                    ui->pushButton_printSessionKeys->setEnabled(false);
                    break;
                case ProceduraVoto::statiProcedura::da_eliminare:
                    ui->pushButton_removeProcedura->setEnabled(true);
                    ui->pushButton_addSchedaVoto->setEnabled(false);
                    ui->pushButton_visualizza_schede->setEnabled(false);
                    ui->pushButton_visualizzaSessioni->setEnabled(false);
                    ui->pushButton_printSessionKeys->setEnabled(false);
                    break;
                case ProceduraVoto::statiProcedura::creazione:
                    ui->pushButton_removeProcedura->setEnabled(true);
                    ui->pushButton_addSchedaVoto->setEnabled(true);

                    ui->pushButton_visualizzaSessioni->setEnabled(true);
                    ui->pushButton_printSessionKeys->setEnabled(true);
                    break;

                default:
                    break;
                }


                ui->widget_azioni_procedura->setEnabled(true);
            }
        }
        else if(ui->tableWidget_lista_procedure->item(row,0)->checkState() == Qt::Unchecked){
            uint idProceduraDeselezionata = ui->tableWidget_lista_procedure->item(currentRow,1)->text().toUInt();
            if(idProceduraDeselezionata == idProceduraSelezionata){
                ui->widget_azioni_procedura->setEnabled(false);
                idProceduraSelezionata = -1;
                statoProceduraSelezionata = ProceduraVoto::statiProcedura::undefined;
                cout << "nessuna Procedura selezionata! " << endl;
            }


        }
    }
}

void MainWindowTecnico::on_pushButton_removeProcedura_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Eliminazione Procedura");
    msgBox.setInformativeText("Stai per eliminare la procedura con id: " + QString::number(idProceduraSelezionata));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.buttons().at(0)->setText("Prosegui");
    msgBox.buttons().at(1)->setText("Annulla");
    int ret = msgBox.exec();

    if (ret==QMessageBox::Cancel){
        return;
    }
    else{
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Attenzione: eliminazione procedura in corso");
        msgBox.setInformativeText("Sei sicuro di voler eliminare la procedura con id: " + QString::number(idProceduraSelezionata) + " ? Tutti i dati di creazione della procedura stanno per essere eliminati. ");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.buttons().at(0)->setText("Si");
        msgBox.buttons().at(1)->setText("NO");
        int ret = msgBox.exec();
        if (ret==QMessageBox::Ok){
            emit deleteProcedura(idProceduraSelezionata);

        }
        else{
            return;
        }
    }
}

void MainWindowTecnico::on_spinBox_numero_schede_editingFinished()
{
    if(ui->spinBox_numero_schede->text()==""){
        ui->spinBox_numero_schede->setValue(1);
    }
}

ProceduraVoto::statiProcedura MainWindowTecnico::getStatoProceduraSelezionata() const
{
    return statoProceduraSelezionata;
}

void MainWindowTecnico::setStatoProceduraSelezionata(const ProceduraVoto::statiProcedura &value)
{
    statoProceduraSelezionata = value;
}

void MainWindowTecnico::on_pushButton_visualizzaSessioni_clicked()
{
    emit needSessioni(idProceduraSelezionata);

}


void MainWindowTecnico::on_pushButton_indietro_toProcedure_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaProcedure);
}

void MainWindowTecnico::on_lineEdit_new_password_textChanged(const QString &arg1)
{
    ui->lineEdit_repeat_new_password->clear();
    if(arg1==""){
        ui->pushButton_confirm_new_password->setEnabled(false);
    }
}

void MainWindowTecnico::on_lineEdit_su_password_textChanged(const QString &arg1)
{
    if(arg1!=""){
        ui->pushButton_confirm_new_password->setEnabled(true);
    }
    else{
        ui->pushButton_confirm_new_password->setEnabled(false);
    }
}

void MainWindowTecnico::on_lineEdit_repeat_new_password_editingFinished()
{
    QString pass1, pass2;
    pass1 = ui->lineEdit_new_password->text();
    pass2 = ui->lineEdit_repeat_new_password->text();
    if(pass1!=pass2){
        ui->label_error_repeat_pass->show();
        ui->pushButton_confirm_new_password->setEnabled(false);
    }
    else{
        ui->label_error_repeat_pass->hide();
        ui->pushButton_confirm_new_password->setEnabled(true);
    }
}

void MainWindowTecnico::on_pushButton_visualizza_schede_clicked()
{
    numeroSchedaDaMostrare = 0;
    ui->pushButton_precedente->setEnabled(false);
    emit needSchedeProcedura(idProceduraSelezionata);
}


void MainWindowTecnico::on_pushButton_backToProcedure_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaProcedure);
}


void MainWindowTecnico::on_pushButton_successiva_clicked()
{
    uint numeroSchedaMostrata = numeroSchedaDaMostrare;
    if(numeroSchedaMostrata < numSchede){
        numeroSchedaDaMostrare++;
        ui->pushButton_precedente->setEnabled(true);
        uint indexUltimaScheda = numSchede-1;
        //cout << indexUltimaScheda << " " << numeroSchedaDaMostrare << endl;
        if(numeroSchedaDaMostrare == indexUltimaScheda){
            //cout << "disabilita successiva"<< endl;
            ui->pushButton_successiva->setEnabled(false);
        }
    }
    mostraScheda();
}

void MainWindowTecnico::on_pushButton_precedente_clicked()
{
    uint numeroSchedaMostrata = numeroSchedaDaMostrare;
    if(numeroSchedaMostrata > 0)
    {
        numeroSchedaDaMostrare--;
        ui->pushButton_successiva->setEnabled(true);
        if(numeroSchedaDaMostrare == 0){
            //cout << "disabilita precedente"<< endl;
            ui->pushButton_precedente->setEnabled(false);
        }
    }
    mostraScheda();
}



void MainWindowTecnico::on_pushButton_printSessionKeys_clicked()
{
    //TODO richiedere al model i dati sulle chiavi di sessione per le postazioni, divise per seggio
    QPrinter printer;
    printer.setPrinterName("desired printer name");
    QPrintDialog dialog(&printer, this);
    if(dialog.exec()==QDialog::Rejected){
        return;
    }else{

        QPainter painter;
        if (! painter.begin(&printer)) { // failed to open file
            qWarning("failed to open file, is it writable?");

        }
        painter.drawText(10, 10, "Test");
        painter.drawText(10, 10, "Test 2"); //la seconda draw text sovrascrive la prima...
        if (! printer.newPage()) {
            qWarning("failed in flushing page to disk, disk full?");

        }
        painter.drawText(10, 10, "Test 2");
        painter.end();
    }
}

void MainWindowTecnico::on_pushButton_eliminaSeggio_clicked()
{
    uint index = ui->comboBox_selezionaSeggioDaRimuovere->currentIndex();

    uint idSeggio = seggiOttenuti.at(index).getIdSeggio();

    QMessageBox msgBox(this);

    QString text = ui->comboBox_selezionaSeggioDaRimuovere->currentText();
    msgBox.setInformativeText("Sei sicuro di volere eliminare il seggio: "+ text + "? Verranno eliminate anche le postazioni associate e i relativi hardware token.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
    msgBox.buttons().at(0)->setText("Elimina Seggio");
    msgBox.buttons().at(1)->setText("Annulla");
    int ret = msgBox.exec();
    if(ret == QMessageBox::Abort){
        return;
    }
    else{
        emit seggioToDelete(idSeggio);
    }


}

void MainWindowTecnico::on_pushButton_backToOperation_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
}

void MainWindowTecnico::on_pushButton_aggiungiSeggio_clicked()
{
    if(ui->lineEdit_denominazioneIndirizzo->text()==""){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Attenzione");
        msgBox.setInformativeText("Compilare tutti i campi.");
        msgBox.exec();
        return;
    }

    QString ipSeggio = ui->comboBox_ipSeggioDisponibili->currentText();
    string ipNuovoSeggio = ipSeggio.toStdString();

    int byte1, byte2, byte3, byte4;
    char dot;
    istringstream s(ipNuovoSeggio);  // input stream that now contains the ip address string

    s >> byte1 >> dot >> byte2 >> dot >> byte3 >> dot >> byte4 >> dot;

    vector <string> ipPostazioni;
    ipPostazioni.push_back(ipNuovoSeggio);

    //genero gli ip delle postazioni
    for(uint i=1; i<=3; i++){
        string ipSeggio = to_string(byte1) + "." + to_string(byte2) + "." + to_string(byte3) + "." +to_string(byte4+i);
        ipPostazioni.push_back(ipSeggio);
    }
    QString desc = ui->lineEdit_denominazioneIndirizzo->text();
    string descrizioneSeggio = desc.toStdString();
    emit postazioniToAdd(ipPostazioni, descrizioneSeggio);
}

void MainWindowTecnico::on_pushButton_testOTP_clicked()
{
    QString sn = ui->lineEdit_SN->text();
    string strSN = sn.toStdString();
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (!(re.exactMatch(sn))){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Errore");
        msgBox.setInformativeText("Serial Number non valido. Il Serial Number deve essere costituito da soli caratteri numerici");
        msgBox.exec();
        return;
    }
    QString username = ui->lineEdit_usernameHT->text();
    string user = username.toStdString();
    QString password = ui->lineEdit_passwordHT->text();
    string pass = password.toStdString();
    QString otp = ui->lineEdit_OTP->text();
    if (!(re.exactMatch(otp))){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Errore");
        msgBox.setInformativeText("OTP non valida, sono ammessi solo caratteri numerici");
        msgBox.exec();
        return;
    }
    string otpStr = otp.toStdString();
    emit testAndRecord(strSN,user,pass,otpStr,idNuovoSeggio);

}

void MainWindowTecnico::on_pushButton_annullaCreazioneSeggio_clicked()
{
    idNuovoSeggio = 0;
    emit rollbackNuovoSeggio();
}

void MainWindowTecnico::on_pushButton_completaCreazioneSeggio_clicked()
{
    emit commitNuovoSeggio();
}

void MainWindowTecnico::on_listWidget_tipoVotanti_itemChanged(QListWidgetItem *item)
{
    QVariant var = item->data(Qt::UserRole);
    string id = var.toString().toStdString();
    cout << "View: Item's id: " << id << endl;
    if(item->checkState()==Qt::CheckState::Checked){
        this->numTipoVotantiChecked++;

    }
    else if (item->checkState()==Qt::CheckState::Unchecked){
        this->numTipoVotantiChecked--;
    }

    cout << "View: numero tipi votanti selezionati: " << numTipoVotantiChecked << endl;
}

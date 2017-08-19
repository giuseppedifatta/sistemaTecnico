#include "mainwindowtecnico.h"
#include "ui_mainwindowtecnico.h"
#include <iostream>
#include <string>
#include <vector>
#include <QMessageBox>

using namespace std;

MainWindowTecnico::MainWindowTecnico(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowTecnico)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
    QStringList tableHeaders;
    tableHeaders << "seleziona" << "idProcedureVoto" << "Descrizione" << "idRP" << "Inizio" << "Termine" << "numero schede" << "stato" ;

    ui->tableWidget_lista_procedure->setHorizontalHeaderLabels(tableHeaders);
    //ui->tableWidget_lista_procedure->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_lista_procedure->resizeColumnsToContents();
    ui->tableWidget_lista_procedure->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->lineEdit_password_tecnico->setEchoMode(QLineEdit::Password);
    ui->lineEdit_new_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_repeat_new_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_su_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_password_rp->setEchoMode(QLineEdit::Password);
    ui->lineEdit_ripeti_password_rp->setEchoMode(QLineEdit::Password);

    cout << "inizializzazione model..." << endl;
    model = new DataManager(this);

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
}


MainWindowTecnico::~MainWindowTecnico()
{
    delete model;
    delete ui;
}



//void MainWindowSeggio::on_token_tableWidget_clicked(const QModelIndex &index)
//{
//    int colonnaCliccata=index.column();
//    if(colonnaCliccata==1)
//            ui->token_tableWidget->removeRow(index.row());
//}


//void MainWindowSeggio::initTableHT(){
//ui->token_tableWidget->verticalHeader()->setVisible(false);
//ui->token_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
//ui->token_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
//QStringList tableHeaders;
//tableHeaders << "S/N hardware token" << "Azione";
//ui->token_tableWidget->setColumnCount(2);
//ui->token_tableWidget->setHorizontalHeaderLabels(tableHeaders);

//ui->token_tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
//ui->token_tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
//QFont font = ui->token_tableWidget->horizontalHeader()->font();
//font.setPointSize(18);
//ui->token_tableWidget->horizontalHeader()->setFont( font );
//ui->token_tableWidget->horizontalHeader()->setStyleSheet(".QHeaderView{}");

//QFont fontItem("Sans Serif",18);
//ui->token_tableWidget->setFont(fontItem);
//}

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
}

void MainWindowTecnico::tecnicoPassAggiornata()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
}

void MainWindowTecnico::messageStoredSchedaVoto()
{
    QMessageBox::information(this,"Success","La scheda di voto è stata correttamente memorizzata nella procedura di voto selezionata.");
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaProcedure);
    delete nuovaScheda;

    pulisciInterfacciaCreazioneScheda();
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
            QString rp = QString::number(rps.at(i).getIdRP()) + ", " + QString::fromStdString(rps.at(i).getNome()) + " " + QString::fromStdString(rps.at(i).getCognome());
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
    statoProceduraSelezionata = "";
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
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,1,item);

        QString descrizione = QString::fromStdString(procedureVoto.at(row).getDescrizione());
        item = new QTableWidgetItem(descrizione);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,2,item);

        uint idRP = procedureVoto.at(row).getIdRP();
        item = new QTableWidgetItem(QString::number(idRP));
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,3,item);

        QString qsInizio = QString::fromStdString(procedureVoto.at(row).getData_ora_inizio());
        item = new QTableWidgetItem(qsInizio);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,4,item);

        QString qsTermine = QString::fromStdString(procedureVoto.at(row).getData_ora_termine());
        item = new QTableWidgetItem(qsTermine);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,5,item);

        uint numSchede = procedureVoto.at(row).getNumSchedeVoto();
        item = new QTableWidgetItem(QString::number(numSchede));
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,6,item);

        QString stato = QString::fromStdString(procedureVoto.at(row).getStato());
        item = new QTableWidgetItem(stato);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_lista_procedure->setItem(rigaAggiunta,7,item);

    }
    ui->tableWidget_lista_procedure->resizeColumnsToContents();
    ui->tableWidget_lista_procedure->horizontalHeader()->setStretchLastSection(true);

    idProceduraSelezionata = -1;
    statoProceduraSelezionata = "";
    ui->widget_azioni_procedura->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaProcedure);
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
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::creazioneSeggio);
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
    ui->dateTimeEdit_data_ora_inizio->clear();
    ui->dateTimeEdit_data_ora_termine->clear();
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
    ui->comboBox_tipo_elezione->clear();
    ui->spinBox_numero_preferenze->clear();

}

void MainWindowTecnico::on_pushButton_addSchedaVoto_clicked()
{
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



    ui->comboBox_tipo_elezione->clear();
    QStringList strList;
    strList << "Senato Accademico" << "Consiglio di Amministrazione" << "Rappresentanti del Personale T.A." << "Consiglio di Corso di Studio" << "Consiglio di Interclasse" << "Consiglio di Dipartimento" ;
    ui->comboBox_tipo_elezione->addItems(strList);

    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::creazioneSchede);

    nuovaScheda = new SchedaVoto();
    nuovaScheda->setIdProceduraVoto(this->idProceduraSelezionata);
}

void MainWindowTecnico::on_pushButton_aggiungi_candidato_clicked()
{
    ui->pushButton_completa_scheda->setEnabled(false);
    ui->pushButton_rimuovi_candidato->setEnabled(false);
    ui->pushButton_rimuovi_gruppo->setEnabled(false);
    ui->pushButton_aggiungi_lista->setEnabled(false);
    ui->pushButton_aggiungi_candidato->setEnabled(false);
    ui->pushButton_conferma_aggiungi->setEnabled(false);

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




        QString lista = ui->comboBox_seleziona_lista_1->currentText();
        string strLista = lista.toStdString();
        if(strLista == ""){
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Vuoi inserire un candidato senza lista? Il candidato verrà aggiunto alla lista <nessuna lista>");
            //msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
            msgBox.buttons().at(0)->setText("Si");
            msgBox.buttons().at(1)->setText("No");
            //msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            if (ret==QMessageBox::Cancel){
                return;
            }
            else{
                strLista = "nessuna lista";
                ui->comboBox_seleziona_lista_2->addItem(QString::fromStdString(strLista));
            }
        }

        QString nome = ui->lineEdit_nome_c->text();
        string strNome = nome.toStdString();
        QString cognome = ui->lineEdit_cognome_c->text();
        string strCognome = cognome.toStdString();
        QDate dataNascita = ui->dateEdit_data_nascita_c->date();
        string dateUTC = dataNascita.toString("yyyy/MM/dd").toStdString();
        cout << dateUTC << endl;
        //QDate date = QDate::fromString(&dateUTC,"yyyy/MM/dd");
        QString luogoNascita = ui->lineEdit_luogo_nascita_c->text();
        string strLuogo = luogoNascita.toStdString();
        QString matricola = ui->lineEdit_matricola->text();
        string strMatricola = matricola.toStdString();

        if(nuovaScheda->addCandidato(strMatricola,strNome,strCognome,strLista,dateUTC,strLuogo)){
            ui->comboBox_seleziona_candidato->addItem(nome + " " + cognome);
            hideBoxAggiungi();
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Il candidato con matricola " + matricola + " è stato aggiunto correttamente.");
            msgBox.exec();
        }
        else{
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Un candidato con matricola " + matricola + " è già presente, impossibile riaggiungerlo o aggiungerne un altro con la stessa matricola, verificare se il numero di matricola è corretto!");
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
    }

}

void MainWindowTecnico::on_pushButton_completa_scheda_clicked()
{
    uint numPref = ui->spinBox_numero_preferenze->text().toUInt();
    nuovaScheda->setNumPreferenze(numPref);
    nuovaScheda->setTipoElezione(ui->comboBox_tipo_elezione->currentIndex());

    if(nuovaScheda->getCandidati().size()>1){
        emit schedaPronta(nuovaScheda);
        cout << "emesso il segnale di scheda pronta" << endl;
    }
    else{
        QMessageBox::information(this,"Error Message","Inserire almeno due candidati prima di completare la scheda");
    }

}

void MainWindowTecnico::on_pushButton_rimuovi_candidato_clicked()
{
    if(ui->comboBox_seleziona_candidato->currentText()!=""){
        int index = ui->comboBox_seleziona_candidato->currentIndex();
        //vector <Candidato> listCandidati = nuovaScheda->getListCandidati();
        //        uint index;
        //        for (uint i = 0; i < listCandidati.size(); i++){
        //            if((listCandidati[i].getNominativo()) == entry.toStdString()){
        //                index = i;
        //                break;
        //            }
        //        }
        nuovaScheda->removeCandidatoFromLista(index);
        ui->comboBox_seleziona_candidato->removeItem(index);
    }
    else{
        QMessageBox msb(this);
        msb.setText("Nulla da eliminare");
        msb.exec();
    }
}

void MainWindowTecnico::on_pushButton_rimuovi_gruppo_clicked()
{
    if(ui->comboBox_seleziona_lista_2->currentText()!=""){
        QString entry = ui->comboBox_seleziona_lista_2->currentText();
        vector <ListaElettorale> listElettorali = nuovaScheda->getListeElettorali();
        uint index;
        for (uint i = 0; i < listElettorali.size(); i++){
            if(listElettorali.at(i).getNome() == entry.toStdString()){
                index = i;
                break;
            }
        }
        vector <Candidato> candidatiDaRimuovere =  nuovaScheda->removeLista(index);
        nuovaScheda->removeCandidatiFromScheda(candidatiDaRimuovere);

        ui->comboBox_seleziona_lista_2->removeItem(index);

        //aggiornamento comboBoxListaCandidati
        ui->comboBox_seleziona_candidato->clear();


        vector <Candidato> listCandidati = nuovaScheda->getCandidati();
        for(unsigned i=0; i< listCandidati.size(); ++i){
            QString str = QString::fromStdString(listCandidati[i].getNome());
            ui->comboBox_seleziona_candidato->addItem(str);
        }

    }
    else{
        QMessageBox msb(this);
        msb.setText("Nulla da eliminare");
        msb.exec();
    }
}

void MainWindowTecnico::on_pushButton_annulla_aggiungi_clicked()
{
    hideBoxAggiungi();
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
    ui->lineEdit_luogo_nascita_c->clear();
    ui->pushButton_annulla_aggiungi->hide();
    ui->pushButton_conferma_aggiungi->hide();


    ui->pushButton_completa_scheda->setEnabled(true);
    ui->pushButton_rimuovi_candidato->setEnabled(true);
    ui->pushButton_rimuovi_gruppo->setEnabled(true);
    ui->pushButton_aggiungi_lista->setEnabled(true);
    ui->pushButton_aggiungi_candidato->setEnabled(true);
}

void MainWindowTecnico::on_lineEdit_nome_c_textChanged(const QString &arg1)
{
    if(arg1==""){
        ui->pushButton_conferma_aggiungi->setEnabled(false);
    }
    else{
        ui->pushButton_conferma_aggiungi->setEnabled(true);

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
        bool periodoMaiRegistratoPrima = nuovaProcedura->getData_ora_inizio()=="";
        //cout << periodoMaiRegistratoPrima << endl;
        if( !periodoMaiRegistratoPrima &&
                (nuovaProcedura->getData_ora_inizio()!=inizio.toString("yyyy/MM/dd hh:mm").toStdString() ||
                 nuovaProcedura->getData_ora_termine()!=termine.toString("yyyy/MM/dd hh:mm").toStdString())){
            QMessageBox msgBox(this);
            msgBox.setInformativeText("Modificare il periodo della procedura? Le sessioni già inserite verranno rimosse ");
            msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
            msgBox.buttons().at(0)->setText("Conferma");
            msgBox.buttons().at(1)->setText("Annulla");
            int ret =msgBox.exec();
            if(ret == QMessageBox::Cancel){
                return;
            }
        }
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
        ui->timeEdit_apertura_sessione->setEnabled(true);
        ui->timeEdit_chiusura_sessione->setEnabled(true);
    }
    else{
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Selezionare un periodo di tempo valido per lo svolgimento della procedura.");
        msgBox.exec();
    }
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
            bool type1 = (dtAperturaSessione >= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getInizio()) ;
            bool type2 = (dtAperturaSessione >= intervalliSessioni.at(i).getFine()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getFine());
            bool type3 = (dtAperturaSessione <= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione >= intervalliSessioni.at(i).getFine());
            bool type4 = (dtAperturaSessione >= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getFine());
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
    ui->dateEdit_data_nascita_rp->setDate(QDate::currentDate().addYears(-18));
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

            idProceduraSelezionata = ui->tableWidget_lista_procedure->item(currentRow,1)->text().toUInt();
            statoProceduraSelezionata = ui->tableWidget_lista_procedure->item(currentRow,7)->text();
            cout << "id Procedura selezionata: " << idProceduraSelezionata << endl;
            unsigned int numberRows = ui->tableWidget_lista_procedure->rowCount();

            for (unsigned int rowIndex = 0; rowIndex < numberRows; rowIndex++){
                if(rowIndex!=currentRow){
                    ui->tableWidget_lista_procedure->item(rowIndex,0)->setCheckState(Qt::Unchecked);
                }
            }

            ui->widget_azioni_procedura->setEnabled(true);
            if(statoProceduraSelezionata!="creazione"){
                ui->pushButton_addSchedaVoto->setEnabled(false);
            }
            if(statoProceduraSelezionata=="in corso" || statoProceduraSelezionata == "conclusa" || statoProceduraSelezionata == "scrutinata"){
                ui->pushButton_removeProcedura->setEnabled(false);
            }

        }
        else if(ui->tableWidget_lista_procedure->item(row,0)->checkState() == Qt::Unchecked){
            uint idProceduraDeselezionata = ui->tableWidget_lista_procedure->item(currentRow,1)->text().toUInt();
            if(idProceduraDeselezionata == idProceduraSelezionata){
                ui->widget_azioni_procedura->setEnabled(false);
                idProceduraSelezionata = -1;
                statoProceduraSelezionata = "";
                cout << "nessuna Procedura selezionata! " << endl;
            }


        }
    }
}

void MainWindowTecnico::on_pushButton_removeProcedura_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Eliminazione Procedura");
    msgBox.setInformativeText("Sei sicuro di voler eliminare la procedura con id: " + QString::number(idProceduraSelezionata));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.buttons().at(0)->setText("Sì");
    msgBox.buttons().at(1)->setText("No");
    int ret = msgBox.exec();

    if (ret==QMessageBox::Cancel){
        return;
    }
    else{
        emit deleteProcedura(idProceduraSelezionata);
    }
}

void MainWindowTecnico::on_spinBox_numero_schede_editingFinished()
{
    if(ui->spinBox_numero_schede->text()==""){
        ui->spinBox_numero_schede->setValue(1);
    }
}

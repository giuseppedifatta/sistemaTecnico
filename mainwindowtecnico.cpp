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
    //on_pushButton_addSchedaVoto_clicked();

    ui->lineEdit_password_tecnico->setEchoMode(QLineEdit::Password);
    ui->lineEdit_new_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_repeat_new_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_su_password->setEchoMode(QLineEdit::Password);

    model = new DataManager(this);

    QObject::connect(this,SIGNAL(tecnicoPass(QString)),model,SLOT(checkPassTecnico(QString)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(passOK()),this,SLOT(showViewSceltaOperazione()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(wrongTecnicoPass()),this,SLOT(passwordErrorMessage()),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(changeTecnicoPass(QString,QString)),model,SLOT(tryChangeTecnicoPass(QString,QString)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(wrongSUpass()),this,SLOT(suPassErrorMessage()),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(tecnicoPassChanged()),this,SLOT(tecnicoPassAggiornata()),Qt::QueuedConnection);

    //qRegisterMetaType< SchedaVoto >( "SchedaVoto" );
    QObject::connect(this,SIGNAL(schedaPronta(SchedaVoto*)),model,SLOT(storeScheda(SchedaVoto*)),Qt::QueuedConnection);
    QObject::connect(model,SIGNAL(storedSchedaVoto()),this,SLOT(messageStoredSchedaVoto()),Qt::QueuedConnection);
}


MainWindowTecnico::~MainWindowTecnico()
{
    delete ui;
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
    ui->comboBox_seleziona_candidato->clear();
    ui->comboBox_seleziona_lista_1->clear();
    ui->comboBox_seleziona_lista_2->clear();
    ui->spinBox_numero_preferenze->setValue(1);
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

    nuovaProcedura = new ProceduraVoto();
    QDateTime d(QDateTime::currentDateTime().date().addDays(1));
    ui->dateTimeEdit_data_ora_inizio->setMinimumDateTime(d);
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::creazioneProcedura);
}

void MainWindowTecnico::on_pushButton_registra_RP_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::registrazioneRP);
}

void MainWindowTecnico::on_pushButton_visualizza_procedure_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::visualizzaProcedure);
}

void MainWindowTecnico::on_pushButton_logout_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::loginUrna);
}

void MainWindowTecnico::on_pushButton_crea_seggio_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::creazioneSeggio);
}

void MainWindowTecnico::on_pushButton_back_scelta_op_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    delete nuovaProcedura;
}

void MainWindowTecnico::on_pushButton_salva_procedura_clicked()
{
    //TODO estrazione dati dalla schermata: descrizione, numero schede, identificativo RP, sessioni
    QString descrizione = ui->lineEdit_descrizione_procedura->text();
    nuovaProcedura->setDescrizione(descrizione.toStdString());
    int numSchede = ui->spinBox_numero_schede->value();
    nuovaProcedura->setNumSchedeVoto(numSchede);
    uint idRP;
    if (ui->comboBox_idRP->currentText()==""){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Selezionare un Responsabile di Procedimento,se non è disponibile alcun Responsabile Procedimento registrarne uno al sistema.");
        return;
    }
    else{
        idRP = ui->comboBox_idRP->currentText().toUInt();
        QString infoRP = QString::fromStdString(nuovaProcedura->getInfoRP(idRP));
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Sta per essere creata la procedura: " + descrizione + ". /n Il responsabile di Procedimento è: " + infoRP);
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Abort){
            return;
        }
    }

    if(intervalliSessioni.empty()){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("Inserire almeno una sessione di voto.");
        return;
    }


    //emettere segnale che la procedura è pronta per la memorizzazione sul db
    emit proceduraPronta(nuovaProcedura);
}

void MainWindowTecnico::on_pushButton_back_scelta_op_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
}

void MainWindowTecnico::on_pushButton_annulla_scheda_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
    cout << "annullamento operazione: cancello la nuova scheda di voto" << endl;
    delete nuovaScheda;
}

void MainWindowTecnico::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(InterfacceTecnico::sceltaOperazione);
}

void MainWindowTecnico::on_pushButton_addSchedaVoto_clicked()
{
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

    //    uint row = ui->tableWidget_lista_procedure->currentRow();
    //    uint idProceduraVoto = ui->tableWidget_lista_procedure->item(row,1)->text().toUInt();
    nuovaScheda->setIdProceduraVoto(1);
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


    vector <string> listListe = nuovaScheda->getListListe();
    for(unsigned i=0; i< listListe.size(); ++i){
        QString str = QString::fromStdString(listListe.at(i));


        ui->comboBox_seleziona_lista_1->addItem(str);
        //}
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
            QMessageBox msgBox;
            msgBox.setText("Vuoi inserire un candidato senza lista?");
            //msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
            msgBox.buttons().at(0)->setText("Si");
            msgBox.buttons().at(1)->setText("No");
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            if (ret==QMessageBox::Cancel){
                return;
            }
        }
        QString nome = ui->lineEdit_nome->text();
        string strNome = nome.toStdString();
        QString cognome = ui->lineEdit_cognome->text();
        string strCognome = cognome.toStdString();
        QDate dataNascita = ui->dateEdit_data_nascita->date();
        string dateUTC = dataNascita.toString("yyyy/MM/dd").toStdString();
        cout << dateUTC << endl;
        //QDate date = QDate::fromString(&dateUTC,"yyyy/MM/dd");
        QString luogoNascita = ui->lineEdit_luogo_nascita->text();
        string strLuogo = luogoNascita.toStdString();

        nuovaScheda->addCandidato(strNome,strLista,strCognome,dateUTC,strLuogo);
        ui->comboBox_seleziona_candidato->addItem(nome + " " + cognome);

        hideBoxAggiungi();
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

    if(nuovaScheda->getListCandidati().size()>1){
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
        nuovaScheda->removeCandidato(index);
        ui->comboBox_seleziona_candidato->removeItem(index);
    }
    else{
        QMessageBox msb;
        msb.setText("Nulla da eliminare");
        msb.exec();
    }
}

void MainWindowTecnico::on_pushButton_rimuovi_gruppo_clicked()
{
    if(ui->comboBox_seleziona_lista_2->currentText()!=""){
        QString entry = ui->comboBox_seleziona_lista_2->currentText();
        vector <string> listAs_g = nuovaScheda->getListListe();;
        uint index;
        for (uint i = 0; i < listAs_g.size(); i++){
            if((listAs_g[i]) == entry.toStdString()){
                index = i;
                break;
            }
        }
        nuovaScheda->removeLista(index);
        ui->comboBox_seleziona_lista_2->removeItem(index);

        //aggiornamento comboBoxListaCandidati
        ui->comboBox_seleziona_candidato->clear();


        vector <Candidato> listCandidati = nuovaScheda->getListCandidati();
        for(unsigned i=0; i< listCandidati.size(); ++i){
            QString str = QString::fromStdString(listCandidati[i].getNome());


            ui->comboBox_seleziona_candidato->addItem(str);

        }

    }
    else{
        QMessageBox msb;
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
    ui->formWidget_candidato->hide();
    ui->lineEdit_nome->clear();
    ui->lineEdit_cognome->clear();
    QDate resetDate = QDate::currentDate();
    ui->dateEdit_data_nascita->setDate(resetDate);
    ui->lineEdit_luogo_nascita->clear();
    ui->pushButton_annulla_aggiungi->hide();
    ui->pushButton_conferma_aggiungi->hide();


    ui->pushButton_completa_scheda->setEnabled(true);
    ui->pushButton_rimuovi_candidato->setEnabled(true);
    ui->pushButton_rimuovi_gruppo->setEnabled(true);
    ui->pushButton_aggiungi_lista->setEnabled(true);
    ui->pushButton_aggiungi_candidato->setEnabled(true);
}


void MainWindowTecnico::on_lineEdit_nome_textChanged(const QString &arg1)
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
        cout << periodoMaiRegistratoPrima << endl;
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

        //memorizza periodo procedura
        string strInizio= inizio.toString("yyyy/MM/dd hh:mm").toStdString();
        cout << strInizio << endl;
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
        msgBox.setInformativeText("La data e ora di inizio procedura deve essere precedente alla data e ora di termine procedura.");
        msgBox.exec();
    }
}

void MainWindowTecnico::on_pushButton_aggiungi_sessione_clicked()
{
    QDate dataSessione = ui->dateEdit_data_sessione->date();
    QTime oraApertura = ui->timeEdit_apertura_sessione->time();
    QTime oraChiusura = ui->timeEdit_chiusura_sessione->time();
    if(oraApertura>=oraChiusura){
        QMessageBox msgBox(this);
        msgBox.setInformativeText("L'ora di apertura sessione deve essere precedente all'ora di chiusura sessione.");
        msgBox.exec();
        return;
    }

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
    nuovaSessione = new SessioneVoto();
    nuovaSessione->setData(dataSessione.toString("yyyy/MM/dd").toStdString());
    nuovaSessione->setOraApertura(oraApertura.toString("hh:mm").toStdString());
    nuovaSessione->setOraChiusura(oraChiusura.toString("hh:mm").toStdString());

    QDateTime dtAperturaSessione = QDateTime(dataSessione,oraApertura);
    QDateTime dtChiusuraSessione = QDateTime(dataSessione,oraChiusura);


    SessioniQt s(dtAperturaSessione,dtChiusuraSessione);
    if(intervalliSessioni.empty()){
        intervalliSessioni.push_back(s);
        nuovaProcedura->addSessione(nuovaSessione);
        ui->comboBox_sessioni_inserite->addItem(dtAperturaSessione.toString("yyyy/MM/dd hh:mm") + " - " + dtChiusuraSessione.toString("yyyy/MM/dd hh:mm"));
    }
    else{
        bool intersezioni = false;
        for(uint i = 0; i < intervalliSessioni.size(); i++){
            bool type1 = (dtAperturaSessione >= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getInizio()) ;
            bool type2 = (dtAperturaSessione >= intervalliSessioni.at(i).getFine()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getFine());
            bool type3 = (dtAperturaSessione <= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione >= intervalliSessioni.at(i).getFine());
            bool type4 = (dtAperturaSessione >= intervalliSessioni.at(i).getInizio()) && (dtChiusuraSessione <= intervalliSessioni.at(i).getFine());
            if(type1 || type2 || type3 ||type4){
                cout << "intersezione sessioni rilevate" << endl;
                intersezioni = true;
                break;
            }
        }
        if (!intersezioni){
            intervalliSessioni.push_back(s);
            nuovaProcedura->addSessione(nuovaSessione);
            ui->comboBox_sessioni_inserite->addItem(dtAperturaSessione.toString("yyyy/MM/dd hh:mm") + " - " + dtChiusuraSessione.toString("yyyy/MM/dd hh:mm"));
        }
        else{
            QMessageBox msgBox(this);
            msgBox.setInformativeText("La sessione da inserire è sovrapposta ad altre sessioni già inserite");
            msgBox.exec();
            return;
        }

    }



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
}

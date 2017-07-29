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
}

void MainWindowTecnico::on_pushButton_salva_procedura_clicked()
{
    //estrazione dati dalla schermata
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
    ui->lineEdit_nominativo->hide();
    ui->label_lista_gruppi_1->hide();
    ui->comboBox_seleziona_gruppo->hide();
    ui->pushButton_annulla_aggiungi->hide();
    ui->pushButton_conferma_aggiungi->hide();

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
    ui->pushButton_aggiungi_gruppo->setEnabled(false);
    ui->pushButton_aggiungi_candidato->setEnabled(false);

    nuovaScheda->setModalitaAdd(SchedaVoto::modoAdd::candidato);

    ui->comboBox_seleziona_gruppo->clear();

    //vector < Associazione > associazioniCorrenti = seggio->getListAssociazioni();
    std::vector <std::string> listAs_g = nuovaScheda->getListAs_g();
    for(unsigned i=0; i< listAs_g.size(); ++i){
        QString str = QString::fromStdString(listAs_g[i]);


        ui->comboBox_seleziona_gruppo->addItem(str);
        //}
    }



    //mostra area aggiunzione candidato
    ui->lineEdit_nominativo->setPlaceholderText("inserisci nominativo candidato");
    ui->lineEdit_nominativo->show();
    ui->label_lista_gruppi_1->show();
    ui->comboBox_seleziona_gruppo->show();
    ui->pushButton_annulla_aggiungi->show();
    ui->pushButton_conferma_aggiungi->show();

}

void MainWindowTecnico::on_pushButton_aggiungi_gruppo_clicked()
{
    ui->pushButton_completa_scheda->setEnabled(false);
    ui->pushButton_rimuovi_candidato->setEnabled(false);
    ui->pushButton_rimuovi_gruppo->setEnabled(false);
    ui->pushButton_aggiungi_gruppo->setEnabled(false);
    ui->pushButton_aggiungi_candidato->setEnabled(false);

    nuovaScheda->setModalitaAdd(SchedaVoto::modoAdd::associazione_gruppo);
    //mostra area aggiunzione associazione_gruppo
    ui->lineEdit_nominativo->setPlaceholderText("inserisci nominativo gruppo/associazione studentesca");
    ui->lineEdit_nominativo->show();
    ui->pushButton_conferma_aggiungi->show();
    ui->pushButton_annulla_aggiungi->show();


}

void MainWindowTecnico::on_pushButton_conferma_aggiungi_clicked()
{
    uint mod = nuovaScheda->getModalitaAdd();
    QString nominativo = ui->lineEdit_nominativo->text();
    std::string strNominativo = nominativo.toStdString();
    if (nominativo == ""){
        std::cerr << "Il campo nominativo non può essere vuoto" << std::endl;
        return;
    }
    if(mod == SchedaVoto::modoAdd::candidato){


        std::string as_g = ui->comboBox_seleziona_gruppo->currentText().toStdString();

        nuovaScheda->addCandidato(strNominativo,as_g);
        ui->comboBox_seleziona_candidato->addItem(nominativo);
    }
    else{
        //modo: associazione_gruppo
        //std::string as_g = ui->lineEdit_nominativo->text().toStdString();
        nuovaScheda->addAs_g(strNominativo);
        ui->comboBox_seleziona_gruppo_2->addItem(nominativo);
    }

    hideBoxAggiungi();
    ui->pushButton_aggiungi_gruppo->setEnabled(true);
    ui->pushButton_aggiungi_candidato->setEnabled(true);

}

void MainWindowTecnico::on_pushButton_completa_scheda_clicked()
{
    uint numPref = ui->spinBox_numero_preferenze->text().toUInt();
    nuovaScheda->setNumPreferenze(numPref);
    if(nuovaScheda->getListCandidati().size()>1){
        emit schedaPronta(nuovaScheda);
    }
    else{
        QMessageBox::information(this,"Error Message","Inserire almeno due candidati prima di completare la scheda");
    }

}

void MainWindowTecnico::on_pushButton_rimuovi_candidato_clicked()
{
    if(ui->comboBox_seleziona_candidato->currentText()!=""){
    QString entry = ui->comboBox_seleziona_candidato->currentText();
    vector <Candidato> listCandidati = nuovaScheda->getListCandidati();;
    uint index;
    for (uint i = 0; i < listCandidati.size(); i++){
        if((listCandidati[i].getNominativo()) == entry.toStdString()){
            index = i;
            break;
        }
    }
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
    if(ui->comboBox_seleziona_gruppo_2->currentText()!=""){
    QString entry = ui->comboBox_seleziona_gruppo_2->currentText();
    vector <std::string> listAs_g = nuovaScheda->getListAs_g();;
    uint index;
    for (uint i = 0; i < listAs_g.size(); i++){
        if((listAs_g[i]) == entry.toStdString()){
            index = i;
            break;
        }
    }
    nuovaScheda->removeAs_g(index);
    ui->comboBox_seleziona_gruppo_2->removeItem(index);

    //aggiornamento comboBoxListaCandidati
    ui->comboBox_seleziona_candidato->clear();


    std::vector <Candidato> listCandidati = nuovaScheda->getListCandidati();
    for(unsigned i=0; i< listCandidati.size(); ++i){
        QString str = QString::fromStdString(listCandidati[i].getNominativo());


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
    ui->lineEdit_nominativo->hide();
    ui->label_lista_gruppi_1->hide();
    ui->comboBox_seleziona_gruppo->hide();
    ui->pushButton_annulla_aggiungi->hide();
    ui->pushButton_conferma_aggiungi->hide();
    ui->lineEdit_nominativo->setText("");

    ui->pushButton_completa_scheda->setEnabled(true);
    ui->pushButton_rimuovi_candidato->setEnabled(true);
    ui->pushButton_rimuovi_gruppo->setEnabled(true);
    ui->pushButton_aggiungi_gruppo->setEnabled(true);
    ui->pushButton_aggiungi_candidato->setEnabled(true);
}


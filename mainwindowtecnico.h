#ifndef MAINWINDOWTECNICO_H
#define MAINWINDOWTECNICO_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <QMessageBox>
#include <QCheckBox>
#include <QPrinter>
#include <QPrintDialog>
#include <sstream>

#include "datamanager.h"
#include "proceduravoto.h"
#include "schedavoto.h"
#include "sessionevoto.h"
#include "candidato.h"
#include "sessioniqt.h"
#include "responsabileprocedimento.h"
#include "infoseggio.h"

using namespace std;

namespace Ui {
class MainWindowTecnico;
}

class MainWindowTecnico : public QMainWindow
{
    Q_OBJECT
signals:
    void tecnicoPass(QString pass);
    void changeTecnicoPass(QString su_pass,QString newTecnicoPass);
    void schedaPronta(SchedaVoto *nuovaScheda);
    void proceduraPronta(ProceduraVoto *nuovaProcedura);
    void rpPronto(ResponsabileProcedimento *nuovoRP);
    void needInfoRPS();
    void needInfoProcedureVoto();
    void deleteProcedura(uint idProceduraSelezionata);
    void needSessioni(uint idProceduraSelezionata);
    void needSchedeProcedura(uint idProceduraSelezionata);
    void checkRangeProcedura(QDateTime inizio, QDateTime fine);
    void needInfoSeggi();
    void seggioToDelete(uint idSeggio);
    void postazioniToAdd(vector <string> ipPostazioni, string descrizioneSeggio);
    void rollbackNuovoSeggio();
    void commitNuovoSeggio();
    void testAndRecord(uint sn,string user,string pass,uint otp,uint idSeggio);
public slots:
    void showViewSceltaOperazione();
    void passwordErrorMessage();
    void suPassErrorMessage();
    void tecnicoPassAggiornata();
    void messageStoredSchedaVoto();
    void messageStoredProcedura();
    void messageRegisteredRP(QString userid);
    void startCreationProcedura(vector <ResponsabileProcedimento> rps);
    void showViewProcedureVoto(QList<ProceduraVoto> procedureVoto);
    void showViewSessioniProcedura(QList<SessioneVoto> sessioni);
    void showViewSchedeProcedura(QList<SchedaVoto> schede);
    void setPeriodoProcedura(QDateTime inizio, QDateTime termine);
    void messageProceduraRangeInUse();
    void showViewSeggi(vector <InfoSeggio> seggiPresenti);
    void addGeneratoriOTP(uint idSeggio);
    void showMessaggeSeggioCreato();
    void showMessageCreazioneSeggioAnnullata();
    void addTokenToTable(uint sn, string user, string pass, uint idSeggio);
    void showMessageTokenNotAvailable();
    void showMessageTestTokenFail();
public:
    explicit MainWindowTecnico(QWidget *parent = 0);
    ~MainWindowTecnico();

    uint getIdProceduraSelezionata() const;
    void setIdProceduraSelezionata(const uint &value);

    ProceduraVoto::statiProcedura getStatoProceduraSelezionata() const;
    void setStatoProceduraSelezionata(const ProceduraVoto::statiProcedura &value);

private slots:
    void on_pushButton_exit_clicked();

    void on_pushButton_login_tecnico_clicked();

    void on_pushButton_change_password_clicked();

    void on_pushButton_back_to_login_urna_clicked();

    void on_pushButton_accedi_clicked();

    void on_pushButton_back_to_login_urna_2_clicked();

    void on_pushButton_confirm_new_password_clicked();

    void on_pushButton_crea_procedura_clicked();

    void on_pushButton_registra_RP_clicked();

    void on_pushButton_visualizza_procedure_clicked();

    void on_pushButton_logout_clicked();

    void on_pushButton_crea_seggio_clicked();

    void on_pushButton_back_scelta_op_clicked();

    void on_pushButton_salva_procedura_clicked();

    void on_pushButton_annulla_procedura_clicked();

    void on_pushButton_annulla_scheda_clicked();

    void on_pushButton_addSchedaVoto_clicked();

    void on_pushButton_aggiungi_candidato_clicked();

    void on_pushButton_aggiungi_lista_clicked();

    void on_pushButton_conferma_aggiungi_clicked();

    void on_pushButton_completa_scheda_clicked();

    void on_pushButton_rimuovi_candidato_clicked();

    void on_pushButton_annulla_aggiungi_clicked();

    void on_pushButton_rimuovi_gruppo_clicked();

    void on_lineEdit_nome_c_textChanged(const QString &arg1);

    void on_lineEdit_nuova_lista_textChanged(const QString &arg1);

    void on_lineEdit_descrizione_procedura_textChanged(const QString &arg1);

    void on_pushButton_memorizza_periodo_procedura_clicked();

    void on_pushButton_aggiungi_sessione_clicked();

    void on_dateTimeEdit_data_ora_inizio_dateTimeChanged(const QDateTime &dateTime);

    void on_timeEdit_apertura_sessione_timeChanged(const QTime &time);

    void on_pushButton_elimina_sessione_clicked();

    void on_pushButton_annulla_rp_clicked();

    void on_pushButton_completa_reg_rp_clicked();

    void on_lineEdit_nome_rp_textChanged(const QString &arg1);

    void on_lineEdit_cognome_rp_textChanged(const QString &arg1);

    void on_lineEdit_password_rp_textChanged(const QString &arg1);

    void on_lineEdit_ripeti_password_rp_textChanged(const QString &arg1);

    void on_pushButton_visualizzaInfoRP_clicked();

    void on_tableWidget_lista_procedure_cellClicked(int row, int column);

    void on_pushButton_removeProcedura_clicked();

    void on_spinBox_numero_schede_editingFinished();

    void on_pushButton_visualizzaSessioni_clicked();

    void on_pushButton_indietro_toProcedure_clicked();

    void on_lineEdit_new_password_textChanged(const QString &arg1);

    void on_lineEdit_su_password_textChanged(const QString &arg1);

    void on_lineEdit_repeat_new_password_editingFinished();

    void on_pushButton_visualizza_schede_clicked();

    void on_pushButton_backToProcedure_clicked();

    void on_listWidget_candidati_itemChanged(QListWidgetItem *item);

    void on_pushButton_successiva_clicked();

    void on_pushButton_precedente_clicked();
    void on_pushButton_printSessionKeys_clicked();

    void on_pushButton_eliminaSeggio_clicked();

    void on_pushButton_backToOperation_clicked();

    void on_pushButton_aggiungiSeggio_clicked();

    void on_pushButton_testOTP_clicked();

    void on_pushButton_annullaCreazioneSeggio_clicked();

    void on_pushButton_completaCreazioneSeggio_clicked();

private:
    Ui::MainWindowTecnico *ui;

    DataManager *model;

    ProceduraVoto *nuovaProcedura;
    SessioneVoto *nuovaSessione;
    SchedaVoto *nuovaScheda;
    ResponsabileProcedimento *nuovoRP;
    uint idProceduraSelezionata;
    ProceduraVoto::statiProcedura statoProceduraSelezionata;
    QString descProceduraSelezionata;

    //per interfaccia visualizzazione schede
    uint numeroSchedaDaMostrare;
    uint numSchede;
    QList <SchedaVoto> schedeOttenute;
    vector<InfoSeggio> seggiOttenuti;
    uint idNuovoSeggio;
    uint numHTaggiunti;

    enum InterfacceTecnico{
        loginUrna,
        loginTecnicoPassword,
        modificaPasswordTecnico,
        sceltaOperazione,
        creazioneProcedura,
        visualizzaProcedure,
        creazioneSchede,
        registrazioneRP,
        creazioneSeggio,
        aggiuntaTokenSeggio,
        visualizzaSessioni,
        visualizzaSchede
    };

    vector <SessioniQt> intervalliSessioni;

    void hideBoxAggiungi();
    void pulisciInterfacciaCreazioneProcedura();
    void pulisciInterfacciaCreazioneScheda();
    void pulisciInterfacciaCreazioneRP();
    void setTables();
    void mostraScheda();
    void initTableHT();
};

#endif // MAINWINDOWTECNICO_H

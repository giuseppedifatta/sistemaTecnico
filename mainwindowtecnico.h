#ifndef MAINWINDOWTECNICO_H
#define MAINWINDOWTECNICO_H

#include <QMainWindow>
#include <vector>

#include "datamanager.h"
#include "proceduravoto.h"
#include "schedavoto.h"
#include "sessionevoto.h"
#include "candidato.h"
#include "sessioniqt.h"
#include "responsabileprocedimento.h"

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
public slots:
    void showViewSceltaOperazione();
    void passwordErrorMessage();
    void suPassErrorMessage();
    void tecnicoPassAggiornata();
    void messageStoredSchedaVoto();
    void messageStoredProcedura();
    void messageRegisteredRP(QString userid);
    void startCreationProcedura(vector <ResponsabileProcedimento> rps);
public:
    explicit MainWindowTecnico(QWidget *parent = 0);
    ~MainWindowTecnico();

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

private:
    Ui::MainWindowTecnico *ui;

    DataManager *model;

    ProceduraVoto *nuovaProcedura;
    SessioneVoto *nuovaSessione;
    SchedaVoto *nuovaScheda;
    ResponsabileProcedimento *nuovoRP;


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
        aggiuntaTokenSeggio
    };

    vector <SessioniQt> intervalliSessioni;

    void hideBoxAggiungi();
    void pulisciInterfacciaCreazioneProcedura();
    void pulisciInterfacciaCreazioneScheda();
    void pulisciInterfacciaCreazioneRP();
};

#endif // MAINWINDOWTECNICO_H

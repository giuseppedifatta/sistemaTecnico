#ifndef MAINWINDOWTECNICO_H
#define MAINWINDOWTECNICO_H

#include <QMainWindow>

#include "datamanager.h"

namespace Ui {
class MainWindowTecnico;
}

class MainWindowTecnico : public QMainWindow
{
    Q_OBJECT
signals:
    void tecnicoPass(QString pass);
    void changeTecnicoPass(QString su_pass,QString newTecnicoPass);
public slots:
    void showViewSceltaOperazione();
    void passwordErrorMessage();
    void suPassErrorMessage();
    void tecnicoPassAggiornata();
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

    void on_pushButton_back_scelta_op_2_clicked();

    void on_pushButton_annulla_scheda_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindowTecnico *ui;

    DataManager *model;
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

};

#endif // MAINWINDOWTECNICO_H

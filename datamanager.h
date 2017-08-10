#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <tinyxml2.h>

#include "schedavoto.h"
#include "proceduravoto.h"
#include "responsabileprocedimento.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = 0);

signals:
    void passOK();
    void wrongTecnicoPass();
    void tecnicoPassChanged();
    void wrongSUpass();
    void storedSchedaVoto();
    void storedRP(QString userid);

public slots:
    void checkPassTecnico(QString pass);
    void tryChangeTecnicoPass(QString su_pass, QString newTecnicoPas);
    void storeScheda(SchedaVoto* scheda);
    void storeRP(ResponsabileProcedimento* rp);
    void storeProcedura(ProceduraVoto* procedura);

private:
    QString tecnicoPass;
    QString suPass;
};

#endif // DATAMANAGER_H

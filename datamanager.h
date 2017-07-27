#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>

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


public slots:
    void checkPassTecnico(QString pass);
    void tryChangeTecnicoPass(QString su_pass, QString newTecnicoPas);

private:
    QString tecnicoPass;
    QString suPass;
};

#endif // DATAMANAGER_H

#ifndef PROCEDURAVOTO_H
#define PROCEDURAVOTO_H
#include <string>
#include <vector>
#include "sessionevoto.h"
#include "responsabileprocedimento.h"
#include <QtCore>

using namespace std;
class ProceduraVoto
{
public:
    ProceduraVoto();
    enum statoProcedura{
        creazione,
        programmata,
        in_corso,
        conclusa,
        scrutinata
    };

    string getDescrizione() const;
    void setDescrizione(const string &value);

    uint getNumSchedeVoto() const;
    void setNumSchedeVoto(const uint &value);

    uint getStato() const;
    void setStato(const uint &value);

    uint getIdRP() const;
    void setIdRP(const uint &value);

    string getData_ora_inizio() const;
    void setData_ora_inizio(const string &value);

    string getData_ora_termine() const;
    void setData_ora_termine(const string &value);

    void addSessione(SessioneVoto *sessione);
    vector<SessioneVoto> getSessioni() const;
    void resetSessioni();
    void removeSessioneByIndex(int index);
    string getInfoRP(uint idRP);
    vector<ResponsabileProcedimento> getRps() const;
    void setRps(const vector<ResponsabileProcedimento> &value);

private:
    string descrizione;
    uint numSchedeVoto;
    uint stato;
    uint idRP;
    string data_ora_inizio;
    string data_ora_termine;
    vector <SessioneVoto> sessioni;
    vector <ResponsabileProcedimento> rps;


};
Q_DECLARE_METATYPE(ProceduraVoto)
#endif // PROCEDURAVOTO_H

#ifndef RESPONSABILEPROCEDIMENTO_H
#define RESPONSABILEPROCEDIMENTO_H

#include <QtCore>
#include <string>
using namespace std;
class ResponsabileProcedimento
{
public:
    ResponsabileProcedimento();
    string getNome() const;
    void setNome(const string &value);

    string getCognome() const;
    void setCognome(const string &value);

    string getLuogoNascita() const;
    void setLuogoNascita(const string &value);

    string getDataNascita() const;
    void setDataNascita(const string &value);

    string getUserid() const;
    void setUserid(const string &value);

    string getPassword() const;
    void setPassword(const string &value);

    uint getIdRP() const;
    void setIdRP(const uint &value);



private:
    string userid;
    string password;
    string nome;
    string cognome;
    string luogoNascita;
    string dataNascita;

    //AVRÀ UN VALORE SOLO QUANDO VERRÀ PRELEVATO DAL DB, NEL MOMENTO IN CUI SI VUOLE CREARE UNA PROCEDURA
    uint idRP;
};
Q_DECLARE_METATYPE(ResponsabileProcedimento)
#endif // RESPONSABILEPROCEDIMENTO_H

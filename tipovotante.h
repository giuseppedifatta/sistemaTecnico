#ifndef TIPOVOTANTE_H
#define TIPOVOTANTE_H
#include <string>
#include <QtCore>
using namespace std;

class TipoVotante
{
public:
    TipoVotante();
    unsigned int getId() const;
    void setId(unsigned int value);

    string getDescrizione() const;
    void setDescrizione(const string &value);

private:
    unsigned int id;
    string descrizione;

};
Q_DECLARE_METATYPE(TipoVotante)
#endif // TIPOVOTANTE_H

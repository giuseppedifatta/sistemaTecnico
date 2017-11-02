#ifndef CHIAVESESSIONE_H
#define CHIAVESESSIONE_H
#include <string>

#include "sessionevoto.h"

using namespace std;

class ChiaveSessione
{
private:
    string sharedKey;
    string ipPostazione;
    uint idSeggio;
    string sedeSeggio;
    SessioneVoto sessioneVoto;

public:
    ChiaveSessione();
    string getSharedKey() const;
    void setSharedKey(const string &value);
    string getIpPostazione() const;
    void setIpPostazione(const string &value);
    uint getIdSeggio() const;
    SessioneVoto getSessioneVoto() const;
    void setSessioneVoto(const SessioneVoto &value);
    void setIdSeggio(const uint &value);
    string getSedeSeggio() const;
    void setSedeSeggio(const string &value);
};

#endif // CHIAVESESSIONE_H

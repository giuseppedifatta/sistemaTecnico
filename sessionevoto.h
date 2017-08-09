#ifndef SESSIONEVOTO_H
#define SESSIONEVOTO_H
#include <string>
using namespace std;
class SessioneVoto
{
public:
    SessioneVoto();


    string getOraApertura() const;
    void setOraApertura(const string &value);

    string getOraChiusura() const;
    void setOraChiusura(const string &value);

    string getData() const;
    void setData(const string &value);

private:
    string oraApertura;
    string oraChiusura;
    string data;
};

#endif // SESSIONEVOTO_H

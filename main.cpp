#include "mainwindowtecnico.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowTecnico w;
    w.show();


    return a.exec();
}


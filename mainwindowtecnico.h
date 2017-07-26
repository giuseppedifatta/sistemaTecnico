#ifndef MAINWINDOWTECNICO_H
#define MAINWINDOWTECNICO_H

#include <QMainWindow>

namespace Ui {
class MainWindowTecnico;
}

class MainWindowTecnico : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindowTecnico(QWidget *parent = 0);
    ~MainWindowTecnico();

private:
    Ui::MainWindowTecnico *ui;
};

#endif // MAINWINDOWTECNICO_H

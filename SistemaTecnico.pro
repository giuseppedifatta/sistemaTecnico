#-------------------------------------------------
#
# Project created by QtCreator 2017-07-26T20:08:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SistemaTecnico
TEMPLATE = app


SOURCES += main.cpp\
        mainwindowtecnico.cpp \
    datamanager.cpp \
    proceduravoto.cpp \
    schedavoto.cpp \
    sessionevoto.cpp \
    candidato.cpp

HEADERS  += mainwindowtecnico.h \
    datamanager.h \
    proceduravoto.h \
    schedavoto.h \
    sessionevoto.h \
    candidato.h

FORMS += \
    mainwindowtecnico.ui

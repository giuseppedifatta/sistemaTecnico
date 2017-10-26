#-------------------------------------------------
#
# Project created by QtCreator 2017-07-26T20:08:15
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SistemaTecnico
TEMPLATE = app


SOURCES += main.cpp\
        mainwindowtecnico.cpp \
    datamanager.cpp \
    proceduravoto.cpp \
    schedavoto.cpp \
    sessionevoto.cpp \
    candidato.cpp \
    sessioniqt.cpp \
    responsabileprocedimento.cpp \
    listaelettorale.cpp \
    infoseggio.cpp \
    openotp_login.cpp \
    tipovotante.cpp

HEADERS  += mainwindowtecnico.h \
    datamanager.h \
    proceduravoto.h \
    schedavoto.h \
    sessionevoto.h \
    candidato.h \
    sessioniqt.h \
    responsabileprocedimento.h \
    listaelettorale.h \
    infoseggio.h \
    openotp_login.h \
    tipovotante.h

FORMS += \
    mainwindowtecnico.ui

LIBS        += -L/usr/local/lib -L/usr/lib -lmysqlcppconn \
            -lcryptopp
INCLUDEPATH += -I/usr/include -I/usr/local/include
               -I/usr/local/include/cppconn



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/i386-linux-gnu/release/ -ltinyxml2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/i386-linux-gnu/debug/ -ltinyxml2
else:unix: LIBS += -L$$PWD/../../../../usr/lib/i386-linux-gnu/ -ltinyxml2

INCLUDEPATH += $$PWD/../../../../usr/lib/i386-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/i386-linux-gnu



unix|win32: LIBS += -lopenotp

unix|win32: LIBS += -lcryptopp

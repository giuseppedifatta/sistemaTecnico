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
    candidato.cpp \
    sessioniqt.cpp \
    responsabileprocedimento.cpp

HEADERS  += mainwindowtecnico.h \
    datamanager.h \
    proceduravoto.h \
    schedavoto.h \
    sessionevoto.h \
    candidato.h \
    sessioniqt.h \
    responsabileprocedimento.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/i386-linux-gnu/release/ -lcryptopp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/i386-linux-gnu/debug/ -lcryptopp
else:unix: LIBS += -L$$PWD/../../../../usr/lib/i386-linux-gnu/ -lcryptopp

INCLUDEPATH += $$PWD/../../../../usr/include/cryptopp
DEPENDPATH += $$PWD/../../../../usr/include/cryptopp

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/i386-linux-gnu/release/libcryptopp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/i386-linux-gnu/debug/libcryptopp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/i386-linux-gnu/release/cryptopp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/i386-linux-gnu/debug/cryptopp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/lib/i386-linux-gnu/libcryptopp.a

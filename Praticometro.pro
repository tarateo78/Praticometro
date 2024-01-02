QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    colonna.cpp \
    colore.cpp \
    connessionedb.cpp \
    criptazione.cpp \
    erogazionecontributi.cpp \
    globalobject.cpp \
    impostazioni.cpp \
    imprese.cpp \
    main.cpp \
    mainwindow.cpp \
    monitoraggi.cpp \
    praticaobject.cpp \
    professionisti.cpp \
    schedadettaglio.cpp \
    signinadmin.cpp \
    statopratiche.cpp \
    utenti.cpp \
    verificaaggiornamenti.cpp

HEADERS += \
    colonna.h \
    colore.h \
    connessionedb.h \
    criptazione.h \
    erogazionecontributi.h \
    globalobject.h \
    impostazioni.h \
    imprese.h \
    mainwindow.h \
    monitoraggi.h \
    praticaobject.h \
    professionisti.h \
    schedadettaglio.h \
    signinadmin.h \
    statopratiche.h \
    utenti.h \
    verificaaggiornamenti.h

FORMS += \
    erogazionecontributi.ui \
    impostazioni.ui \
    imprese.ui \
    mainwindow.ui \
    monitoraggi.ui \
    professionisti.ui \
    schedadettaglio.ui \
    signinadmin.ui \
    statopratiche.ui \
    utenti.ui \
    verificaaggiornamenti.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Immagini.qrc

DISTFILES += \
    icone_32/appunti.png \
    icone_32/arancione.png \
    icone_32/blu.png \
    icone_32/calendario.png \
    icone_32/cartella.png \
    icone_32/cassetti.png \
    icone_32/chiave.png \
    icone_32/clessidra.png \
    icone_32/esporta.png \
    icone_32/giallo.png \
    icone_32/grafico.png \
    icone_32/lavori.png \
    icone_32/lente.png \
    icone_32/lucchetto-chiave.png \
    icone_32/puntina.png \
    icone_32/rosso.png \
    icone_32/squadra.png \
    icone_32/verde.png \
    icone_32/vero.png \
    icone_32/vioala.png

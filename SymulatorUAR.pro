QT       += core gui printsupport widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

SOURCES += \
    ARX.cpp \
    GeneratorSygnalu.cpp \
    MenadzerSymulacji.cpp \
    Reg_PID.cpp \
    SymulatorUAR.cpp \
    Testy_UAR.cpp \
    W_SIECIOWA/DialogPolaczenia.cpp \
    W_SIECIOWA/MenadzerSieci.cpp \
    W_SIECIOWA/Serializacja.cpp \
    W_SIECIOWA/clientMW.cpp \
    W_SIECIOWA/myTCPclient.cpp \
    W_SIECIOWA/myTCPserver.cpp \
    W_SIECIOWA/serverMW.cpp \
    ZarzadzanieCzasem.cpp \
    dialogarx.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    ARX.h \
    GeneratorSygnalu.h \
    MenadzerSymulacji.h \
    Reg_PID.h \
    SymulatorUAR.h \
    Testy_UAR.h \
    W_SIECIOWA/DialogPolaczenia.h \
    W_SIECIOWA/MenadzerSieci.h \
    W_SIECIOWA/Serializacja.h \
    W_SIECIOWA/clientMW.h \
    W_SIECIOWA/myTCPclient.h \
    W_SIECIOWA/myTCPserver.h \
    W_SIECIOWA/serverMW.h \
    ZarzadzanieCzasem.h \
    dialogarx.h \
    json.hpp \
    mainwindow.h \
    qcustomplot.h \
    TestUtils.h

FORMS += \
    W_SIECIOWA/DialogPolaczenia.ui \
    dialogarx.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    konfiguracja.json

QMAKE_CXXFLAGS += /bigobj

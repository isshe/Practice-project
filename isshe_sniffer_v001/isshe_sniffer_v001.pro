#-------------------------------------------------
#
# Project created by QtCreator 2017-01-15T16:14:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = isshe_sniffer_v001
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 20170124-isshe
LIBS += -lpcap -lpthread

SOURCES += main.cpp\
        mainwindow.cpp \
    select_interface.cpp \
    capturethread.cpp \
    networklayout.cpp \
    datalinklayout.cpp \
    transportlayout.cpp \
    tcpheader.cpp \
    protocolheader.cpp \
    hashtable.cpp \
    statistics.cpp \
    pieview.cpp \
    statisticsmw.cpp

HEADERS  += mainwindow.h \
    select_interface.h \
    capturethread.h \
    main.h \
    networklayout.h \
    datalinklayout.h \
    transportlayout.h \
    tcpheader.h \
    vlan.h \
    ospf.h \
    protocolheader.h \
    hashtable.h \
    statistics.h \
    pieview.h \
    statisticsmw.h

FORMS    += mainwindow.ui \
    select_interface.ui \
    tcpheader.ui \
    protocolheader.ui \
    statistics.ui \
    statisticsmw.ui

RESOURCES += \
    image.qrc

DISTFILES += \
    Readme.md

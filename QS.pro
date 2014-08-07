#-------------------------------------------------
#
# Project created by QtCreator 2014-08-02T20:43:25
#
#-------------------------------------------------

QT       += core gui \
         multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QS
TEMPLATE = app


SOURCES += main.cpp\
        qswindow.cpp \
    pianokey.cpp \
    scoreitem.cpp \
    wavchannel.cpp \
    qsscene.cpp

HEADERS  += qswindow.h \
    pianokey.h \
    scoreitem.h \
    wavchannel.h \
    qsscene.h

FORMS    += qswindow.ui

RESOURCES += \
    qs.qrc

ICON = icon.icns

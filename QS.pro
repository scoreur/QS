#-------------------------------------------------
#
# Project created by QtCreator 2014-08-02T20:43:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QS
TEMPLATE = app


SOURCES += main.cpp\
        qswindow.cpp \
    pianokey.cpp \
    scoreitem.cpp

HEADERS  += qswindow.h \
    pianokey.h \
    scoreitem.h

FORMS    += qswindow.ui

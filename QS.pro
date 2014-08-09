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
    qsscene.cpp \
    qspreset.cpp \
    wavscene.cpp \
    scorescene.cpp \
    staffscene.cpp \
    wavframe.cpp \
    qsview.cpp

HEADERS  += qswindow.h \
    pianokey.h \
    scoreitem.h \
    qsscene.h \
    qspreset.h \
    wavscene.h \
    scorescene.h \
    staffscene.h \
    wavframe.h \
    qsview.h

FORMS    += qswindow.ui \
    qspreset.ui

RESOURCES += \
    qs.qrc

ICON = icon.icns

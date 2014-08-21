#-------------------------------------------------
#
# Project created by QtCreator 2014-08-02T20:43:25
#
#-------------------------------------------------

QT       += core gui \
         multimedia multimediawidgets \
         network\


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QS
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        qswindow.cpp \
    pianokey.cpp \
    Score/scoreitem.cpp \
    Score/scorescene.cpp \
    qsscene.cpp \
    qspreset.cpp \
    Wave/wavscene.cpp \
    Wave/wavframe.cpp \
    Staff/staffscene.cpp \
    qsview.cpp \
    qsplayer.cpp \
    Staff/staffitem.cpp \
    Core/spectrum.cpp

HEADERS  += qswindow.h \
    pianokey.h \
    Score/scoreitem.h \
    Score/scorescene.h \
    qsscene.h \
    qspreset.h \
    Wave/wavscene.h \
    Wave/wavframe.h \
    Staff/staffscene.h \
    qsview.h \
    qsplayer.h \
    Staff/staffitem.h \
    Core/spectrum.h

FORMS    += qswindow.ui \
    qspreset.ui

RESOURCES += \
    qs.qrc

ICON = icon.icns

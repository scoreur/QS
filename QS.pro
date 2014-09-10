#-------------------------------------------------
#
# Project created by QtCreator 2014-08-02T20:43:25
#
#-------------------------------------------------

QT       += core gui \
         multimedia multimediawidgets \
         network\


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = QS
TEMPLATE = app
CONFIG += c++11 \
        ordered

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
    Core/spectrum.cpp \
    midiparser.cpp \
    Wave/wavfile.cpp

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
    Core/spectrum.h \
    midiparser.h \
    decorator.h \
    MIDIFile.hpp \
    Wave/lame.h \
    Wave/wavfile.h

INCLUDEPATH += $$PWD

DEFINES += TOUCH_OPTIMIZED_NAVIGATION
# Please do not modify the following two lines. Required for deployment.
include(html5applicationviewer/html5applicationviewer.pri)
qtcAddDeployment()


FORMS    += qswindow.ui \
    qspreset.ui

RESOURCES += \
    qs.qrc

ICON = icon.icns

win32: LIBS += -L$$PWD/Wave/lame/win/ -lmp3lame
else:unix: LIBS += -L$$PWD/Wave/lame/mac/ -lmp3lame

INCLUDEPATH += $$PWD/Wave
DEPENDPATH += $$PWD/Wave

win32-g++:PRE_TARGETDEPS += $$PWD/Wave/lame/win/libmp3lame.a
else:win32:!win32-g++: PRE_TARGETDEPS += $$PWD/Wave/lame/win/mp3lame.lib
else:unix: PRE_TARGETDEPS += $$PWD/Wave/lame/mac/libmp3lame.a

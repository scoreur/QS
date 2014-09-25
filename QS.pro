-------------------------------------------------
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
    General/qswindow.cpp \
    General/qsview.cpp \
    General/qsscene.cpp \
    pianokey.cpp \
    qspreset.cpp \
    qsplayer.cpp \
    Wave/wavscene.cpp \
    Wave/wavframe.cpp \
    Score/scoreitem.cpp \
    Score/scorescene.cpp \
    Staff/staffscene.cpp \
    Staff/staffitem.cpp \
    Core/spectrum.cpp \
    midiparser.cpp \
    Wave/wavfile.cpp \
    qsrecorder.cpp

HEADERS  += General/qswindow.h \
    General/qsview.h \
    General/qsscene.h \
    qspreset.h \
    pianokey.h \
    qsplayer.h \
    Wave/wavscene.h \
    Wave/wavframe.h \
    Wave/lame.h \
    Wave/wavfile.h\
    Score/scoreitem.h \
    Score/scorescene.h \
    Staff/staffscene.h \ 
    Staff/staffitem.h \
    Core/spectrum.h \
    midiparser.h \
    decorator.h \
    MIDIFile.hpp \
    qsrecorder.h

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

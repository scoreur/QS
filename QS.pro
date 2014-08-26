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
    Core/spectrum.cpp \
    midiparser.cpp

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
    midiparser.h

DEFINES += TOUCH_OPTIMIZED_NAVIGATION
# Please do not modify the following two lines. Required for deployment.
include(html5applicationviewer/html5applicationviewer.pri)
qtcAddDeployment()


FORMS    += qswindow.ui \
    qspreset.ui

RESOURCES += \
    qs.qrc

ICON = icon.icns



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/MIDI/release/ -lMIDI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/MIDI/debug/ -lMIDI
else:unix: LIBS += -L$$PWD/MIDI/ -lMIDI

INCLUDEPATH += $$PWD/MIDI
DEPENDPATH += $$PWD/MIDI

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/MIDI/release/libMIDI.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/MIDI/debug/libMIDI.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/MIDI/release/MIDI.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/MIDI/debug/MIDI.lib
else:unix: PRE_TARGETDEPS += $$PWD/MIDI/libMIDI.a

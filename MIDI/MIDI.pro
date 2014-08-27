#-------------------------------------------------
#
# Project created by QtCreator 2014-08-26T16:17:24
#
#-------------------------------------------------

QT       -= gui

TARGET = MIDI
TEMPLATE = lib
CONFIG += staticlib \
          c++11

SOURCES += \
    ChannelControllerEvent.cpp \
    ChannelEvent.cpp \
    Chunk.cpp \
    DataBuffer.cpp \
    Event.cpp \
    File.cpp \
    Header.cpp \
    MetaEvent.cpp \
    MetaGenericEvent.cpp \
    MetaNumberEvent.cpp \
    MetaTextEvent.cpp \
    Track.cpp \
    types.cpp

HEADERS += MIDI.h \
    ChannelControllerEvent.h \
    ChannelEvent.h \
    Chunk.h \
    DataBuffer.h \
    Defines.h \
    Event.h \
    File.h \
    Header.h \
    MetaEvent.h \
    MetaGenericEvent.h \
    MetaNumberEvent.h \
    MetaTextEvent.h \
    Track.h \
    types.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

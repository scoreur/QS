#include "qsplayer.h"
#include <QFile>
#include <QDebug>


QSPlayer::QSPlayer(QString fileName, QWidget *parent) :
    QThread(parent),
    player(new QMediaPlayer(this,QMediaPlayer::StreamPlayback)),
    audioOut(0), file(0)


{
    player->setMedia(QUrl::fromLocalFile(fileName));
    player->setNotifyInterval(200);



}
QSPlayer::~QSPlayer(){
        player->setMedia(QUrl());//discard the media
        qDebug()<<"discard media to prevent error";
        delete file;
}

void QSPlayer::run(){
    //qDebug()<<"player:"<<player->position();
    QFile *file = new QFile;
    file->setFileName(QString("/Users/user/Documents/compile/Qt5/QS/sound/keyboard/piano_35.wav"));
    file->open(QIODevice::ReadOnly);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleRate(44100);
    format.setChannelCount(2);
    qDebug()<<format.isValid();
    audioOut = new QAudioOutput(format, parent());

    //audioOut->start(file);
    qDebug()<<exec();
}

/*
void QSPlayer::playerConnect(QSWindow *win){
    connect(player, SIGNAL(positionChanged(qint64)), win,SLOT(positionChanged(qint64)));


}*/

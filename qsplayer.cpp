#include "qsplayer.h"

#include <QDebug>


QSPlayer::QSPlayer(QString fileName, QWidget *parent) :
    QThread(parent), player(new QMediaPlayer(this,QMediaPlayer::VideoSurface))


{
    player->setMedia(QUrl::fromLocalFile(fileName));

}
QSPlayer::~QSPlayer(){
        player->setMedia(QUrl());//discard the media
        qDebug()<<"discard media to prevent error";
}

void QSPlayer::run(){
    //qDebug()<<"player:"<<player->position();
    qDebug()<<exec();
}

/*
void QSPlayer::playerConnect(QSWindow *win){
    connect(player, SIGNAL(positionChanged(qint64)), win,SLOT(positionChanged(qint64)));


}*/

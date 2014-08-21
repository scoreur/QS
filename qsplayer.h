#ifndef QSPLAYER_H
#define QSPLAYER_H
#include <QSlider>
#include <QAbstractButton>
#include <QMediaPlayer>
#include <QThread>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QFile>

class QSWindow;

class QSPlayer : public QThread
{
    Q_OBJECT
public:
    explicit QSPlayer(QString fileName,QWidget *parent = 0);
    ~QSPlayer();
    QMediaPlayer *player;
    QAudioOutput *audioOut;
    QAudioFormat format;
    QFile *file;
    void run();
    //void playerConnect(QSlider *pos_slid, QAbstractButton *btn);

signals:

public slots:
    
};

#endif // QSPLAYER_H

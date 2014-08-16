#ifndef QSPLAYER_H
#define QSPLAYER_H
#include <QSlider>
#include <QAbstractButton>
#include <QMediaPlayer>
#include <QThread>

class QSWindow;

class QSPlayer : public QThread
{
    Q_OBJECT
public:
    explicit QSPlayer(QString fileName,QWidget *parent = 0);
    ~QSPlayer();
    QMediaPlayer *player;

    void run();
    //void playerConnect(QSlider *pos_slid, QAbstractButton *btn);

signals:

public slots:
    
};

#endif // QSPLAYER_H

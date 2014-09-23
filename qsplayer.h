#ifndef QSPLAYER_H
#define QSPLAYER_H
#include <QSlider>
#include <QAbstractButton>
#include <QMediaPlayer>
#include <QThread>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QFile>
#include <QLabel>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>

class QSWindow;
class QSLrcLine;

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
    QSLrcLine *lrc;
    void run();
    //void playerConnect(QSlider *pos_slid, QAbstractButton *btn);

signals:

public slots:
    
};

class QSLrcLine : public QLabel{
    Q_OBJECT
public:
    explicit QSLrcLine(): QLabel(0, Qt::ToolTip){

        resize(600,50);
        move(QApplication::screens()[0]->availableGeometry().center()+QPoint(-100,100));
        setWindowOpacity(0.8);
        setMouseTracking(false);



        lrcPalette.setColor(QPalette::Background,defaultColor);
        lrcPalette.setColor(QPalette::WindowText, QColor(QColor(255,255,0)));
        setText("Nothing on You Baby");
        setFont(QFont("Times [Adobe]", 16 , QFont::Bold));
        setPalette(lrcPalette);
        setStyleSheet ("border:1px groove gray;border-radius:10px;padding:2px 4px;");

    }

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *ev){
        qDebug()<<ev->pos()<<ev->screenPos();
        mousePos = ev->pos();
    }

    void mouseMoveEvent(QMouseEvent *event){

        move(event->screenPos().toPoint()-mousePos);
    }
    void mouseReleaseEvent(QMouseEvent *event){
        mousePos = QPoint();
    }

private:
    static QPoint mousePos;
    static QColor activeColor;
    static QColor defaultColor;
    QPalette lrcPalette;
};


#endif // QSPLAYER_H

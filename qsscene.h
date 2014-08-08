#ifndef QSSCENE_H
#define QSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>
#include <QString>
#include <QDebug>


class KeyScene : public QGraphicsScene
{

public:
    KeyScene(QObject *parent, QGraphicsView *view);
    ~KeyScene(){
        qDebug()<<"keyScene removed!";
    }

};

class WavScene : public QGraphicsScene
{

public:
    WavScene(QObject *parent, QGraphicsView *view, QString fileName = "");
    ~WavScene(){
        qDebug()<<"wavScene removed!";
    }

    quint32 load(QString fileName, quint32 _off, quint32 _len);
    QString Name(){
        return name;
    }
    static qreal secondsPerView;
    static quint32 wavBufferSize;
private:
    short *ldata, *rdata;
    quint32 offset, len;
    QString name;
};

class ScoreScene : public QGraphicsScene
{

public:
    ScoreScene(QObject *parent, QGraphicsView *view, QString fileName = "");
    ~ScoreScene(){
        qDebug()<<"scoreScene removed!";
    }
    quint32 load(QString fileName);
    QString Name(){
        return name;
    }

private:
    std::vector<uchar> note;
    std::vector<quint8> dura;
    quint32 len;
    QString name;
};



#endif // QSSCENE_H

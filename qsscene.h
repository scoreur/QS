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

    uint32_t load(QString fileName, uint32_t _off, uint32_t _len);

    static qreal secondsPerView;
private:
    short *ldata, *rdata;
    uint32_t offset, len;
};

class ScoreScene : public QGraphicsScene
{

public:
    ScoreScene(QObject *parent, QGraphicsView *view, QString fileName = "");
    ~ScoreScene(){
        qDebug()<<"scoreScene removed!";
    }
    uint32_t load(QString fileName);

private:
    std::vector<uchar> note;
    std::vector<uint8_t> dura;
    uint32_t len;

};



#endif // QSSCENE_H

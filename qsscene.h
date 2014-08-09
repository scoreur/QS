#ifndef QSSCENE_H
#define QSSCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <QString>
#include <QAction>


class QGraphicsItem;
class QGraphicsView;

class KeyScene : public QGraphicsScene
{

public:
    KeyScene(QGraphicsView *view, QObject *parent);
    ~KeyScene();
private:
    QGraphicsItem *board;
};

class QSScene : public QGraphicsScene{

public:
    explicit QSScene(QGraphicsView *view, QString fileName = "");
    ~QSScene();
    QString Name() const{
        return name;
    }
    QAction *Opened() const{
        return act;
    }
    void setName(QString fileName);
    virtual quint32 load(QString fileName) = 0;
    virtual quint32 store(QString fileName) = 0;

private:
    QString name;
    QAction *act;
};

class WavScene : public QSScene
{
public:
    WavScene(QGraphicsView *view, QString fileName = "");
    ~WavScene();

    quint32 load(QString fileName){
        return load(fileName, 0, wavBufferSize);
    }
    quint32 store(QString fileName){return 0;}
    quint32 load(QString fileName, quint32 _offset, quint32 _len);
    static qreal secondsPerView;
    static quint32 wavBufferSize;
private:
    short *ldata, *rdata;
    quint32 offset, len;
};

class ScoreScene : public QSScene
{

public:
    ScoreScene(QGraphicsView *view, QString fileName = "");
    ~ScoreScene();
    quint32 load(QString fileName);
    quint32 store(QString fileName){return 0;}

private:
    QVector<uchar> note;
    QVector<quint8> dura;
    quint32 len;
};

class StaffScene : public QSScene{
public:
    StaffScene(QGraphicsView *view, QString fileName);
    ~StaffScene();
    quint32 load(QString fileName){return 0;}
    quint32 store(QString fileName){return 0;}
private:

};


#endif // QSSCENE_H

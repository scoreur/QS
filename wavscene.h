#ifndef WAVSCENE_H
#define WAVSCENE_H

#include "qsscene.h"
#include <QGraphicsRectItem>

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
    static qreal accuracy;
    static quint16 sampleps;
    static quint32 wavBufferSize;
protected:
    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
private:
    short *ldata, *rdata;
    quint32 offset, len;
};

class WavChannel : QGraphicsRectItem{

public:
    WavChannel(QGraphicsScene *scene, QGraphicsItem *parent = 0);
    ~WavChannel(){}


};

#endif // WAVSCENE_H

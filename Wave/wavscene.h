#ifndef WAVSCENE_H
#define WAVSCENE_H

#include "qsscene.h"
#include "wavframe.h"
#include <QGraphicsRectItem>

class WavScene : public QSScene
{
public:
    WavScene(QGraphicsView *view, QString fileName = "");
    ~WavScene();

    quint32 load(QString fileName){
        return load0(this, fileName);
    }
    quint32 store(QString fileName);
    static quint32 load0(WavScene* obj, QString fileName);//for multithread
    Q_INVOKABLE void loadReady();


    static qreal secondsPerView;
    static qreal accuracy;
    static quint16 sampleps;
    static quint32 wavBufferSize;
    bool isMoving;

protected:
   // void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
private:

    quint32 len;
    WavFile *wavFile;

};


class WavChannel : QGraphicsRectItem{

public:
    WavChannel(int x, int y, int w, int h, QGraphicsScene *scene, QGraphicsItem *parent = 0);
    ~WavChannel(){}

    friend class WavScene;

protected:
    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

};


#endif // WAVSCENE_H

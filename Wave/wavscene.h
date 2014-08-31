#ifndef WAVSCENE_H
#define WAVSCENE_H

#include "qsscene.h"
#include "wavframe.h"
#include <QGraphicsRectItem>
#include <QPicture>


class Spectrum;
class SpectrumGraph;

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
    bool showSpect;
    SpectrumGraph *spect;

public slots:
    void getMoving(qreal fracPos);

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

class SpectrumGraph: public QGraphicsRectItem{
public:
    SpectrumGraph(Spectrum *spectrum, QGraphicsScene *scene, QGraphicsItem *parent = 0);
    SpectrumGraph(QGraphicsScene *scene);
    ~SpectrumGraph(){}
    void fresh(Spectrum *spectrum);
    void fresh(qint16 data[], quint32 num, quint8 mode=0);

    friend class WavScene;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QVector<QPointF> points;
    QPicture pic;
};


#endif // WAVSCENE_H

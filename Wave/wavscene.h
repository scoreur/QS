#ifndef WAVSCENE_H
#define WAVSCENE_H

#include "General/qsscene.h"
#include "wavframe.h"
#include <QGraphicsRectItem>
#include <QPicture>
#include <QLabel>


class Spectrum;
class SpectrumGraph;

class WavScene : public QSScene
{
public:
    WavScene(QGraphicsView *view, const QString &fileName = "");
    WavScene(QGraphicsView *view, const WavFile &_wavFile, const QString &fileName);
    ~WavScene();

    quint32 load(QString fileName);
    quint32 store(QString fileName);
    static quint32 load0(WavScene* obj, QString fileName);//for multithread
    Q_INVOKABLE void loadReady();


    static qreal secondsPerView;
    static qreal accuracy;
    static quint16 sampleps;
    static quint32 wavBufferSize;
    static QColor selectedColor;
    bool isMoving;
    bool showSpect;
    bool selected;
    SpectrumGraph *spect;


public slots:
    void getMoving(qreal fracPos);

protected:
   // void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:

    quint32 len;
    WavFile *wavFile;
    QGraphicsItem *channel[2];
    qreal selectPos0;
    QGraphicsRectItem *grabber;
    QLabel *mouseTime;

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

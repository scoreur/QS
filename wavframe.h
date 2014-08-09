#ifndef WAVFRAME_H
#define WAVFRAME_H
#include <QPainter>
#include <QGraphicsItem>

class WavFrame: public QGraphicsItem{

public:
    WavFrame(quint32 _datasize, short *_data, qreal _intv, qreal _amp);
    ~WavFrame(){
        delete []data;
    };

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath shape() const;
    QRectF boundingRect() const{
        return bound;
    }

    static quint16 framesize;
signals:

public slots:

private:

    quint32 datasize;
    qreal interval;
    qreal amplitude;
    QPointF *data;
    QRectF bound;

};


#endif // WAVFRAME_H

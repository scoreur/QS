#ifndef WAVCHANNEL_H
#define WAVCHANNEL_H
#include <QPainter>
#include <QGraphicsItem>

class wavChannel: public QGraphicsItem{

public:
    wavChannel(qulonglong _datasize, short *_data, qreal _intv, qreal _amp);
    ~wavChannel(){
        delete []data;
    };

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    QPainterPath shape() const;
signals:

public slots:

private:

    qulonglong datasize;
    qreal interval;
    qreal amplitude;
    QPointF *data;
    const QRectF bound;

};


#endif // WAVCHANNEL_H

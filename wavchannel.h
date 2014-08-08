#ifndef WAVCHANNEL_H
#define WAVCHANNEL_H
#include <QPainter>
#include <QGraphicsItem>

class wavChannel: public QGraphicsItem{

public:
    wavChannel(quint32 _datasize, short *_data, qreal _intv, qreal _amp);
    ~wavChannel(){
        delete []data;
    };

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath shape() const;
    QRectF boundingRect() const{
        return bound;
    }
signals:

public slots:

private:

    quint32 datasize;
    qreal interval;
    qreal amplitude;
    QPointF *data;
    QRectF bound;

};


#endif // WAVCHANNEL_H

#include "wavchannel.h"
#include <QDebug>
#include <QCursor>

wavChannel::wavChannel(qulonglong _datasize, short *_data, qreal _intv, qreal _amp):
    datasize(_datasize), interval(_intv), amplitude(_amp),
    bound(QRectF(0,-_amp, _intv*_datasize,_amp*2)){
    setCursor(QCursor(Qt::IBeamCursor));
    data = new QPointF[datasize];
    qreal temp_x = 0;
    for(qulonglong i=0;i<datasize;++i){
        data[i].setX(temp_x);
        data[i].setY(-amplitude /(1<<15)*_data[i]);//正负2的15次幂
        temp_x += interval;
    }
    //qDebug()<<"construct channel: "<<temp_x;
}

void wavChannel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen(Qt::darkGreen);

    painter->setPen(pen);

    if(datasize>0)
        painter->drawPolyline(data, datasize);

}

QRectF wavChannel::boundingRect() const{
    return bound;
}
QPainterPath wavChannel::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}
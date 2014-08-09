
#include "wavframe.h"
#include <QDebug>


WavFrame::WavFrame(quint32 _datasize, short *_data, qreal _intv, qreal _amp):
    datasize(_datasize), interval(_intv), amplitude(_amp),
    bound(QRectF(0,-_amp, _intv*_datasize,_amp*1.5)){

    data = new QPointF[datasize];
    qreal temp_x = 0;
    for(quint32 i=0;i<datasize;++i){
        data[i].setX(temp_x);
        data[i].setY(-amplitude /(1<<15)*_data[i]);//正负2的15次幂
        temp_x += interval;
    }
    //qDebug()<<"construct channel: "<<temp_x;
}

// static member
quint16 WavFrame::framesize = 200;

void WavFrame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(QPen(Qt::darkGreen));
    if(datasize>0)
        painter->drawPolyline(data, datasize);

}

QPainterPath WavFrame::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

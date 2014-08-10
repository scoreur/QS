
#include "wavframe.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>


WavFrame::WavFrame(quint32 _len, short *_data, qreal _intv, qreal _width, qreal _amp):
    datasize((quint32)(_width/_intv)),
    interval(_intv), amplitude(_amp),
    bound(QRectF(0,-_amp, _width,_amp*2.0)){

    data = new QPointF[datasize];
    qreal d = _len/(qreal)datasize;
    for(quint32 i=0;i<datasize;++i){
        quint32 j = (quint32)(i*d);
        data[i] = QPointF(j*interval, -amplitude /(1<<15)*_data[j]);
        //2 to the 15th power as maximum
    }
    int k = 1;
    while(k<_len-100){
    while(k<_len-1 && (_data[k]-_data[k-1]<=5 || _data[k+1]-_data[k]>=-5)
          &&(_data[k]-_data[k-1]>=-5 || _data[k+1]-_data[k]<=5))++k;
    pdata.push_back(QPointF(k*interval, -amplitude /(1<<15)*_data[k])); ++k;
    }
    qDebug()<<"array"<<datasize<<"vector:"<<pdata.size();
}

// static member
quint16 WavFrame::framesize = 200;

void WavFrame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(QPen(Qt::darkGreen));

    //painter->drawPolyline((QPointF*)pdata.begin(), pdata.size());
    painter->drawPolyline(data, datasize);
}

QPainterPath WavFrame::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

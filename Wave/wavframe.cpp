
#include "../qspreset.h"
#include "wavframe.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "../Core/spectrum.h"

double ampExpand(double in){
    int sgn = in>0? 1: -1;
    in /= (1<<15);
    //return sgn * ::pow(in*in, 0.2);
    return in;
}

WavFrame::WavFrame(quint32 _len, short *_data, qreal _intv, qreal _width, qreal _amp):
    datasize((quint32)(_width/_intv)),
    interval(_intv), amplitude(_amp),
    bound(QRectF(0,-_amp, _width,_amp*2.0)){

    data = new QPointF[datasize];
    qreal d = _len/(qreal)datasize;
    data[0]=QPointF(0, -amplitude * ampExpand(_data[0]));
    data[1]=QPointF(d, -amplitude * ampExpand(_data[0]));
    data[datasize-2]=QPointF(_len*interval, -amplitude * ampExpand(_data[datasize-1]));
    data[datasize-2]=QPointF(_len*interval-d, -amplitude * ampExpand(_data[datasize-1]));
    for(quint32 i=2;i<datasize-2;++i){
        quint32 j = (quint32)(i*d+0.5);
        qreal delta = i*d-j;
        data[i] = QPointF(i*d*interval, -amplitude * ampExpand(interpolation3P(delta, _data[j-1],_data[j],_data[j+1])));
        //2 to the 15th power as maximum
    }
    qDebug()<<"array "<<datasize;
    int k = 1;
    while(k<_len-100){
    while(k<_len-1 && (_data[k]-_data[k-1]<=5 || _data[k+1]-_data[k]>=-5)
          &&(_data[k]-_data[k-1]>=-5 || _data[k+1]-_data[k]<=5))++k;
    pdata.push_back(QPointF(k*interval, -amplitude*ampExpand(_data[k]))); ++k;
    }
    qDebug()<<"vector:"<<pdata.size();
}

// static member
quint16 WavFrame::framesize = 200;

void WavFrame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen pen(QSPreset::wavForegroundColor);
    pen.setWidth(1);
    painter->setPen(pen);

    //painter->drawPolyline((QPointF*)pdata.begin(), pdata.size());
    painter->drawPolyline(data, datasize);
}

QPainterPath WavFrame::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

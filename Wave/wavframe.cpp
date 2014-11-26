
#include "qspreset.h"
#include "wavframe.h"
#include <fstream>
#include <QDebug>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "Core/spectrum.h"


qreal ampScale(qreal in){
    //int sgn = in>0? 1: -1;
    in /= WavFile::maxAmp;
    //return sgn * ::pow(in*in, 0.2);
    return in;
}

bool WavFrame::useInterpolation3P = false;

WavFrame::WavFrame(quint32 _len, short *_data, qreal _intv, qreal _width, qreal halfheight, quint8 step):
    datasize((quint32)(_width/_intv)),
    interval(_intv), amplitude(halfheight), data(0),
    bound(QRectF(0,-halfheight, _width,halfheight*2.0)){

    fresh(_data, _len, step);
    qDebug()<<"frame added";
}

void WavFrame::processData(qint16 *_data, quint32 num, quint8 step){
    if(data != 0) delete[] data;
    data = new QPointF[datasize];

    //accurate step length
    qreal d = num/(qreal)datasize;
    //begin processing
    if(useInterpolation3P){
        data[0]=QPointF(0, -amplitude * ampScale(_data[0]));
        data[1]=QPointF(d, -amplitude * ampScale(_data[0]));
        data[datasize-1]=QPointF(num*interval, -amplitude * ampScale(_data[step*(datasize-1)]));
        data[datasize-2]=QPointF(num*interval-d, -amplitude * ampScale(_data[step*(datasize-1)]));
        for(quint32 i=2;i<datasize-2;++i){

            quint32 j = (quint32)(i*d+0.5);
            qreal delta = i*d-j;
            qreal amptmp = interpolation3P(delta, _data[step*(j-1)],_data[step*j],_data[step*(j+1)]);
            if(i%100 == 0) qDebug()<<j<<"&"<<delta<<"&"<<amptmp<<"&"<<_data[step*(j+1)];
            data[i] = QPointF(i*d*interval, -amplitude * ampScale(amptmp));
            //2 to the 15th power as maximum
        }
    }else{
        for(quint32 i=0;i<datasize;++i){
            quint32 j = (quint32)(i*d);
            data[i] = QPointF(j*interval, -amplitude * ampScale(_data[j]));
        }
    }


    qDebug()<<"data processed";


    /*
    quint32 k = 1;
    while(k<_len-100){
    while(k<_len-1 && (_data[step*k]-_data[step*(k-1)]<=5 || _data[step*(k+1)]-_data[step*k]>=-5)
          &&(_data[step*k]-_data[step*(k-1)]>=-5 || _data[step*(k+1)]-_data[step*k]<=5))++k;
    pdata.push_back(QPointF(k*interval, -amplitude*ampScale(_data[step*k]))); ++k;
    }
    qDebug()<<"vector:"<<pdata.size();
    */ // pick extreme value
}

void WavFrame::constructPic(){
    QPainter pt;
    pt.begin(&pic);
    QPen pen(QSPreset::wavForegroundColor);
    pen.setWidth(1);
    pt.setPen(pen);
    //pt->drawPolyline((QPointF*)pdata.begin(), pdata.size());
    pt.drawPolyline(data, datasize);
    pt.drawLine(0,0, bound.width(), 0);
    //qDebug()<<"pic constructed";
    pt.end();
}

void WavFrame::fresh(qint16 _data[], quint32 num, quint8 step){
    processData(_data, num, step);
    constructPic();
}

// static member
quint16 WavFrame::framesize = 200;

void WavFrame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPicture(0,0,pic);


}

QPainterPath WavFrame::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}



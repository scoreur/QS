#include <QGraphicsView>
#include <QGraphicsItem>
#include <QFile>
//#include <QUrl>
#include <QDebug>
#include "wavframe.h"
#include "wavscene.h"
#include "../Core/spectrum.h"
#include <QGraphicsSceneWheelEvent>
#include <cmath>
#include <thread>


qreal WavScene::secondsPerView = 5;
qreal WavScene::accuracy = 0.05;
quint16 WavScene::sampleps = 44100;
quint32 WavScene::wavBufferSize = 1<<22;
QColor WavScene::backgroundColor = QColor(51, 204, 102, 240);
/// @brief constructor of WavScene

WavScene::WavScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName), ldata(new short[1]), rdata(new short[1]),
      offset(0), len(0)

{
    setItemIndexMethod(NoIndex);
    //view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //view->setCacheMode(QGraphicsView::CacheBackground);
    if(!fileName.isEmpty()){
        std::thread th((WavScene::load0), (WavScene*)this, fileName, offset, wavBufferSize);
        th.detach();
    }


    if(fileName.isEmpty())
        setName("Untitled.wav");

}
WavScene::~WavScene(){
    delete []ldata; delete []rdata;
    qDebug()<<"wavScene removed!";
}


quint32 WavScene::load0(WavScene* obj, QString fileName, quint32 _off, quint32 _len){

    QFile wavin(fileName);
    wavin.open(QFileDevice::ReadOnly);
    if(wavin.isOpen())
    {
        quint32 len0 = 0;
        wavin.seek(16);
        wavin.read((char*)&len0, 4);//get 16 or 18
        len0 += 24;
        wavin.seek(len0);
        wavin.read((char*)&len0, 4);
        qDebug()<<"the wave file length: "<<len0/44100.0/4.0<<"s";
        if(_off + _len<len0){
            len0 = _len;
        }else{
            _off = 0;
        }
        delete []obj->ldata; delete []obj->rdata;
        obj->ldata = new short[len0];
        obj->rdata = new short[len0];
        wavin.seek(44+_off);
        for(quint32 i=0;i<len0;++i){
            wavin.read((char*)(obj->ldata+i),2);
            wavin.read((char*)(obj->rdata+i),2);
        }
        wavin.close();
        qDebug()<<"load successful: "<<len0;
        obj->len = len0;
        obj->loadReady();
        return len0;
    }else
        return 0;
}

void WavScene::loadReady(){
    if(len != 0){
        qreal temp_h = views()[0]->height();
        qreal temp_w = views()[0]->width();
        qreal scene_w = temp_w/sampleps/secondsPerView * len;
        setSceneRect(0,0, scene_w, temp_h);
        WavFrame * frame = new WavFrame(len, ldata,
            accuracy, scene_w, temp_h/4.0);
        frame->setPos(0,temp_h/4.0);
        addItem(frame);
        frame = new WavFrame(len, rdata,
            accuracy, scene_w, temp_h/4.0);
        frame->setPos(0,temp_h/4.0*3);
        addItem(frame);

    }
}

quint32 WavScene::store(QString fileName){
    return fileName.size();
}

void WavScene::drawForeground(QPainter *painter, const QRectF &rect){

    painter->drawText(rect,Qt::AlignLeft|Qt::AlignTop, QString("WAVEFORM"));

}
void WavScene::drawBackground(QPainter *painter, const QRectF &rect){
    painter->setBrush(QBrush(backgroundColor));
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

    painter->setPen(QPen(Qt::green));
    painter->drawLine(0,0.50*height(), width(),0.50*height());
    painter->drawLine(0,0.25*height(), width(),0.25*height());
    painter->drawLine(0,0.75*height(), width(),0.75*height());


}
void WavScene::wheelEvent(QGraphicsSceneWheelEvent *event){
    //qDebug()<<event->delta();

}

void WavScene::DFT(QPointF out[], quint32 pos, quint32 halflen){
    if(len < 2*halflen) return;
    if(pos < halflen) pos = halflen;
    if(pos > len - halflen) pos = len - halflen;
    //64 points

    for(int k=0; k<88; ++k){
        qreal del_i = halflen/32.0;
        qreal x_temp = 0, y_temp = 0;
        qreal fr = pianoFreq[k] * 2 * 3.14159 /44100;
        for(int i = -31; i<=32; ++i){
            int j = pos + (int)(i * del_i);
            y_temp += (ldata[j] * ::cos(j * fr));
            x_temp += (ldata[j] * ::sin(j * fr));
        }
        out[k].setY(::sqrt(x_temp*x_temp+y_temp*y_temp)/(qreal)(1<<15)*130);
    }



}

WavChannel::WavChannel(QGraphicsScene *scene, QGraphicsItem *parent):
    QGraphicsRectItem(parent)
{
    scene->addItem(this);
}

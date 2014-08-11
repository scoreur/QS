#include <QGraphicsView>
#include <QGraphicsItem>
#include <QFile>
//#include <QUrl>
#include <QDebug>
#include "wavframe.h"
#include "wavscene.h"
#include <QGraphicsSceneWheelEvent>

qreal WavScene::secondsPerView = 5;
qreal WavScene::accuracy = 0.05;
quint16 WavScene::sampleps = 44100;
quint32 WavScene::wavBufferSize = 1<<22;

/// @brief constructor of WavScene

WavScene::WavScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName), ldata(new short[1]), rdata(new short[1]),
      offset(0), len(wavBufferSize)

{
    setItemIndexMethod(NoIndex);
    //view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //view->setCacheMode(QGraphicsView::CacheBackground);



    if(fileName != "" && load(fileName, offset, wavBufferSize) != 0){
        qreal temp_h = (qreal)view->height();
        qreal temp_w = (qreal)view->width();
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
    if(fileName.isEmpty())
        setName("Untitled.wav");

}
WavScene::~WavScene(){
    delete []ldata; delete []rdata;
    qDebug()<<"wavScene removed!";
}


quint32 WavScene::load(QString fileName, quint32 _off, quint32 _len){

    QFile wavin(fileName, this);
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
        delete []ldata; delete []rdata;
        ldata = new short[len0];
        rdata = new short[len0];
        wavin.seek(44+_off);
        for(quint32 i=0;i<len0;++i){
            wavin.read((char*)(ldata+i),2);
            wavin.read((char*)(rdata+i),2);
        }
        wavin.close();
        qDebug()<<"load successful: "<<len0;
        return len = len0;
    }else
        return 0;
}

void WavScene::drawForeground(QPainter *painter, const QRectF &rect){
    ;
}
void WavScene::drawBackground(QPainter *painter, const QRectF &rect){
    painter->setBrush(QBrush(QColor(0,150,0,100)));
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

    painter->setPen(QPen(Qt::darkGreen));
    painter->drawLine(0,0.50*height(), width(),0.50*height());
    painter->drawLine(0,0.25*height(), width(),0.25*height());
    painter->drawLine(0,0.75*height(), width(),0.75*height());


}
void WavScene::wheelEvent(QGraphicsSceneWheelEvent *event){
    //qDebug()<<event->delta();

}

WavChannel::WavChannel(QGraphicsScene *scene, QGraphicsItem *parent):
    QGraphicsRectItem(parent)
{
    scene->addItem(this);
}

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QFile>
#include <fstream>
//#include <QUrl>
#include <cmath>
#include <thread>
#include <QDebug>
#include "wavframe.h"
#include "wavscene.h"
#include "Core/spectrum.h"
#include "qspreset.h"
#include "lame.h"


qreal WavScene::secondsPerView = 2.5;
qreal WavScene::accuracy = 0.05;
quint16 WavScene::sampleps = 44100;
quint32 WavScene::wavBufferSize = 1<<22;
// @brief constructor of WavScene

void display(mp3data_struct *md){//display header info
    qDebug()<<"nchannels:" << md->stereo;
    qDebug()<<"samplerate:" << md->samplerate;
    qDebug()<<"bitrate:" << md->bitrate;
    qDebug()<<"framesize:" << md->framesize;
}
const int buffersize = 4000;
int test_lame(char* in, char* out){
       std::ifstream fd(in);
       if(!fd){
           qDebug()<<"open file %s error!"<<in;
           return 1;
       }
       std::ofstream fw(out);

       qDebug()<<in<<"->"<<out;
       mp3data_struct md;
       hip_t mfile = hip_decode_init();

       unsigned char mbuffer[buffersize];
       short pcm_l[40000]; short pcm_r[40000];
       fd.read((char*)mbuffer, buffersize);

       int nout = hip_decode_headers( mfile, mbuffer, buffersize, pcm_l, pcm_r, &md);
       display(&md);
       int i=0;
       for(i=0; i<nout; ++i){
           fw.write((char*)(pcm_l+i),2);
           fw.write((char*)(pcm_r+i), 2);
       }

       int totallen = nout;
       while( fd.read((char*)mbuffer, buffersize)){
           nout = hip_decode(mfile, mbuffer, buffersize, pcm_l, pcm_r);
           qDebug()<<"out:"<<nout;
           if(nout == -1) { qDebug()<<"ERROR!"; hip_decode_exit(mfile); return 3;}
           totallen += nout;
           for(i=0; i<nout; ++i){
               fw.write((char*)(pcm_l+i),2);
               fw.write((char*)(pcm_r+i), 2);
           }
       }
       qDebug()<<"Exit: /total lenth sec:"<<hip_decode_exit(mfile)<<totallen/44100.0;
       fd.close();
       fw.close();
       return 0;

}

WavScene::WavScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName), isMoving(false),
      len(0), wavFile(0)

{
    setItemIndexMethod(NoIndex);
    //view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //view->setCacheMode(QGraphicsView::CacheBackground);
    if(!fileName.isEmpty()){
        std::thread th((WavScene::load0), (WavScene*)this, fileName);
        th.detach();
    }
    if(fileName.isEmpty())
        setName("Untitled.wav");

    //WavFile::test();
    //qDebug()<<test_lame("/Users/user/Music/test.mp3","/Users/user/Music/test.pcm");


}
WavScene::~WavScene(){
    if(wavFile != 0) delete wavFile;

    qDebug()<<"wavScene removed!";
}

//static
quint32 WavScene::load0(WavScene* obj, QString fileName){
    obj->wavFile = new WavFile(fileName.toStdString().c_str());
    if(obj->wavFile->length == 0)
        return (obj->len = 0);
    obj->len = obj->wavFile->length/obj->wavFile->nChannels();
    qDebug()<<"load successful: "<<obj->len;
    obj->loadReady();
    return obj->len;
}

void WavScene::loadReady(){
    if(len != 0){
        qreal temp_h = views()[0]->height();
        qreal temp_w = views()[0]->width();
        qreal scene_w = temp_w/sampleps/secondsPerView * len;
        setSceneRect(0,0, scene_w, temp_h);
        WavChannel *channel = new WavChannel(0,-temp_h/2.0/wavFile->nChannels(), scene_w, temp_h/wavFile->nChannels()
                                             , this);
        channel->setPos(0,temp_h/2.0/wavFile->nChannels());
        WavFrame * frame = new WavFrame(len, wavFile->data,
            accuracy, scene_w, temp_h/2.0/wavFile->nChannels(), wavFile->nChannels());
        frame->setParentItem(channel);
        if(wavFile->nChannels()==2){
            channel = new WavChannel(0,-temp_h/4.0,scene_w,temp_h/2.0, this);
            channel->setPos(0,temp_h/4.0*3);
            frame = new WavFrame(len, wavFile->data+1,
                accuracy, scene_w, temp_h/4.0, 2);
            frame->setParentItem(channel);
        }

    }
}

quint32 WavScene::store(QString fileName){
    return fileName.size();
}

void WavScene::drawBackground(QPainter *painter, const QRectF &rect){
    painter->setBrush(QSPreset::wavBackgroundColor);
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

}

void WavScene::wheelEvent(QGraphicsSceneWheelEvent *event){
    Q_UNUSED(event)
    //qDebug()<<event->delta();

}

WavChannel::WavChannel(int x, int y, int w, int h, QGraphicsScene *scene, QGraphicsItem *parent):
    QGraphicsRectItem(parent)
{

    scene->addItem(this);
    setRect(x,y,w,h);


}

void WavChannel::drawForeground(QPainter *painter, const QRectF &rect){
    QPixmap p(100,20);
    QPainter pt(&p);
    pt.setPen(QPen(Qt::red));
    pt.drawText(p.rect(),Qt::AlignCenter, QString("WAVEFORM"));
    if(! ((WavScene*)scene())->isMoving)
        painter->drawPixmap(QRect(rect.topLeft().toPoint(),p.size()),p);
}

void WavChannel::drawBackground(QPainter *painter, const QRectF &rect){

    painter->setPen(QSPreset::wavForegroundColor);

    painter->drawLine(0,boundingRect().height()/2,boundingRect().width(),boundingRect().height()/2);
}

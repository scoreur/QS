#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QFile>
#include <QVector>
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


int test_lame(char *in, char *out);

WavScene::WavScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName), isMoving(false),
      len(0), wavFile(0), showSpect(false), spect(new SpectrumGraph(this))
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

    //Spectrum::test();


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
    quint8 step = obj->wavFile->nChannels();
    obj->len = obj->wavFile->length/step;
    //add channel & frames
    qreal temp_h = obj->views()[0]->height();
    qreal temp_w = obj->views()[0]->width();
    qreal scene_w = temp_w/obj->wavFile->sampleps()/WavScene::secondsPerView * obj->len;
    obj->setSceneRect(0,0, scene_w, temp_h);
    WavChannel *channel = new WavChannel(0,-temp_h/2.0/step, scene_w, temp_h/step
                                         , obj);
    channel->setPos(0,temp_h/2.0/step);
    WavFrame * frame = new WavFrame(obj->len, obj->wavFile->data,
        accuracy, scene_w, temp_h/2.0/step, step);
    frame->setParentItem((QGraphicsItem*)channel);
    if(step==2){
        channel = new WavChannel(0,-temp_h/4.0,scene_w,temp_h/2.0, obj);
        channel->setPos(0,temp_h/4.0*3);
        frame = new WavFrame(obj->len, obj->wavFile->data+1,
            accuracy, scene_w, temp_h/4.0, 2);
        frame->setParentItem((QGraphicsItem*)channel);
    }
    obj->loadReady();
    return obj->len;
}

void WavScene::loadReady(){
    if(len != 0){
        qDebug()<<"load successful: "<<len;
    }
}

quint32 WavScene::store(QString fileName){
    if(wavFile != 0)
        wavFile->save(fileName.toStdString().c_str());
    return wavFile->length;
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

void WavScene::getMoving(qreal fracPos){
    if(spect != 0){
        spect->setPos(fracPos*sceneRect().width(), 0);
        quint32 l = fracPos * (wavFile->length-6000);
        spect->fresh(wavFile->data+l, 6000, 3);
    }

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

void lame_display(mp3data_struct *md){//display header info
    qDebug()<<"nchannels:" << md->stereo;
    qDebug()<<"samplerate:" << md->samplerate;
    qDebug()<<"bitrate:" << md->bitrate;
    qDebug()<<"framesize:" << md->framesize;
}
const int buffersize = 4000;
int test_lame(char* in, char* out){
       std::ifstream wavIn(in);
       if(!wavIn){
           qDebug()<<"open file %s error!"<<in;
           return 1;
       }
       std::ofstream wavOut(out);

       qDebug()<<in<<"->"<<out;
       mp3data_struct md;
       hip_t mfile = hip_decode_init();

       unsigned char mbuffer[buffersize];
       short pcm_l[40000]; short pcm_r[40000];
       wavIn.read((char*)mbuffer, buffersize);

       int nout = hip_decode_headers( mfile, mbuffer, buffersize, pcm_l, pcm_r, &md);
       lame_display(&md);
       int i=0;
       for(i=0; i<nout; ++i){
           wavOut.write((char*)(pcm_l+i),2);
           wavOut.write((char*)(pcm_r+i), 2);
       }

       int totallen = nout;
       while( wavIn.read((char*)mbuffer, buffersize)){
           nout = hip_decode(mfile, mbuffer, buffersize, pcm_l, pcm_r);
           qDebug()<<"out:"<<nout;
           if(nout == -1) { qDebug()<<"ERROR!"; hip_decode_exit(mfile); return 3;}
           totallen += nout;
           for(i=0; i<nout; ++i){
               wavOut.write((char*)(pcm_l+i),2);
               wavOut.write((char*)(pcm_r+i), 2);
           }
       }
       qDebug()<<"Exit: /total lenth sec:"<<hip_decode_exit(mfile)<<totallen/44100.0;
       wavIn.close();
       wavOut.close();
       return 0;

}

qreal cmplx_norm(const cmplx & c){
    qreal rl = c.real();
    if(rl>-1e-6 && rl<1e-6) rl = 0;
    qreal im = c.imag();
    if(im>-1e-6 && im<1e-6) im = 0;
    return ::sqrt(rl*rl+im*im);
}

SpectrumGraph::SpectrumGraph(Spectrum *spectrum, QGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsRectItem(parent){
    scene->addItem(this);
    setRect(QSPreset::wavViewRect);
    fresh(spectrum);

}
SpectrumGraph::SpectrumGraph(QGraphicsScene *scene): QGraphicsRectItem(0){
    scene->addItem(this);
    setRect(QSPreset::wavViewRect);
}

void SpectrumGraph::fresh(Spectrum *spectrum){
    if(! points.empty()) points.clear();
    qreal temp_x = 0, temp_y = 150, step_x = rect().width()/spectrum->vallen;
    for(std::vector<cmplx>::iterator iter=spectrum->val.begin();
        iter != spectrum->val.end(); ++iter, temp_x+=step_x){
        points<<QPointF(temp_x, temp_y - cmplx_norm(*iter)/(1<<15)*600);
    }
    QPainter pt;
    pt.begin(&pic);
    pt.setPen(QPen(Qt::red));
    pt.drawPolyline(&points[0], points.size());
    pt.end();
    scene()->update(sceneBoundingRect());
}
void SpectrumGraph::fresh(qint16 data[], quint32 num, quint8 mode){
    Spectrum sp(data, num, mode);
    fresh(&sp);
}

void SpectrumGraph::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(((WavScene*)scene())->showSpect)
        painter->drawPicture(10,10,pic);

}


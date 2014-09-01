#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneWheelEvent>
#include <QFile>
#include <QDir>
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



qreal WavScene::secondsPerView = 2;
qreal WavScene::accuracy = 0.05;
quint16 WavScene::sampleps = 44100;
quint32 WavScene::wavBufferSize = 1<<22;
// @brief constructor of WavScene




WavScene::WavScene(QGraphicsView *view, const QString &fileName)
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

    //Spectrum::test();

}
WavScene::WavScene(QGraphicsView *view, const WavFile &_wavFile, const QString &fileName):QSScene(view, fileName), isMoving(false),
    len(_wavFile.length/_wavFile.nChannels()), wavFile(new WavFile(_wavFile)), showSpect(false), spect(new SpectrumGraph(this)){
    setItemIndexMethod(NoIndex);

    loadReady();
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
    obj->loadReady();
    return obj->len;
}

void WavScene::loadReady(){
    if(len != 0){
        //add channel & frames
        qreal temp_h = views()[0]->height();
        qreal temp_w = views()[0]->width();
        qreal scene_w = temp_w/wavFile->sampleps()/WavScene::secondsPerView * len;
        quint8 step = wavFile->nChannels();
        setSceneRect(0,0, scene_w, temp_h);
        WavChannel *channel = new WavChannel(0,-temp_h/2.0/step, scene_w, temp_h/step
                                             , this);
        channel->setPos(0,temp_h/2.0/step);
        WavFrame * frame = new WavFrame(len, wavFile->data,
            accuracy, scene_w, temp_h/2.0/step, step);
        frame->setParentItem((QGraphicsItem*)channel);
        if(step==2){
            channel = new WavChannel(0,-temp_h/4.0,scene_w,temp_h/2.0, this);
            channel->setPos(0,temp_h/4.0*3);
            frame = new WavFrame(len, wavFile->data+1,
                accuracy, scene_w, temp_h/4.0, 2);
            frame->setParentItem((QGraphicsItem*)channel);
        }
        qDebug()<<"load successful: "<<len;
        spect->fresh(wavFile->data, 6000, 3, step);
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
        spect->fresh(wavFile->data+l, 6000, 3, (quint8)wavFile->nChannels());
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
    qreal temp_x = 0, temp_y = 180, step_x = rect().width()/spectrum->vallen;
    for(std::vector<cmplx>::iterator iter=spectrum->val.begin();
        iter != spectrum->val.end(); ++iter, temp_x+=step_x){
        points<<QPointF(temp_x, temp_y - cmplx_norm(*iter)/(1<<15)*750);
    }
    QPainter pt;
    pt.begin(&pic);
    pt.setPen(QPen(Qt::red));
    pt.drawPolyline(&points[0], points.size());
    pt.end();
    scene()->update(sceneBoundingRect());
}
void SpectrumGraph::fresh(qint16 data[], quint32 num, quint8 mode, quint8 step){
    Spectrum sp(data, num, mode, step);
    fresh(&sp);
}

void SpectrumGraph::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(((WavScene*)scene())->showSpect)
        painter->drawPicture(10,10,pic);

}




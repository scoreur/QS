#include "qsscene.h"
#include "pianokey.h"
#include "scoreitem.h"
#include "wavchannel.h"
#include <fstream>




KeyScene::KeyScene(QObject *parent, QGraphicsView *view)
    : QGraphicsScene(parent)
{

    view->setScene(this);
    setSceneRect(view->geometry());
    setItemIndexMethod(BspTreeIndex);
    view->setRenderHint(QPainter::Antialiasing);
    view->setUpdatesEnabled(true);

    qreal temp_x = PianoKey::halfspacing, temp_y = view->y()+2;
    for(uchar i=0;i<88;++i){
        PianoKey *pianoKey = new PianoKey(i);
        pianoKey->setPos(temp_x, temp_y);
        addItem(pianoKey);
        if(i%12==2 || i%12 ==7) //B, E
            temp_x += PianoKey::halfspacing;
        temp_x += PianoKey::halfspacing;
    }
}

qreal WavScene::secondsPerView = 0.05;

/// @brief constructor of WavScene
WavScene::WavScene(QObject *parent, QGraphicsView *view, QString fileName)
    : QGraphicsScene(parent), offset(0), len(100000)
{
    view->setScene(this);

    setItemIndexMethod(NoIndex);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //view->setCacheMode(QGraphicsView::CacheBackground);
    //view->setSceneRect(sceneRect());
    setBackgroundBrush(QColor(0,150,0,100));

    if(fileName == ""){
        setSceneRect(view->geometry());

    }else{
        len = load(fileName, offset, len);
        if(len != 0){
            qreal temp_h = (qreal)view->height();
            qreal temp_w = (qreal)view->width();
            wavChannel * channel = new wavChannel(len, ldata,
                temp_w/44100.0/secondsPerView, 0.2500*temp_h);
            channel->setPos(0,0.2500 * temp_h);
            addItem(channel);
            channel = new wavChannel(len, rdata,
                temp_w/44100.0/secondsPerView, 0.2500*temp_h);
            channel->setPos(0,0.7500 * temp_h);
            addItem(channel);

            setSceneRect(0,0, channel->boundingRect().width(),0.99*temp_h);

        }else
            setSceneRect(view->geometry());
    }
    addRect(sceneRect(), QPen(Qt::black));
    addLine(0,0.50*height(), width(),0.50*height(), QPen(Qt::darkGreen));
    addLine(0,0.25*height(), width(),0.25*height(), QPen(Qt::darkGreen));
    addLine(0,0.75*height(), width(),0.75*height(), QPen(Qt::darkGreen));


}

ScoreScene::ScoreScene(QObject *parent, QGraphicsView *view, QString fileName)
    : QGraphicsScene(parent)
{
    view->setScene(this);
    setSceneRect(view->geometry());
    setItemIndexMethod(BspTreeIndex);
    view->setRenderHint(QPainter::Antialiasing);

    ScoreItem *score;
    if(fileName != ""){
        len = load(fileName);
    }else
        len = 0;

    if(len == 0){
        len = 88;
        for(uint32_t i=0;i<len;++i)
            note.push_back(i);
    }
    qreal temp_x = ScoreItem::halfnotewidth + 10, temp_y = ScoreItem::halfnoteheight + 10;
    for(uint32_t i=0;i<len;++i){//test

        score = new ScoreItem(note[i]);
        score->setPos(temp_x, temp_y);
        addItem(score);

        temp_x += ScoreItem::notespacing;
        if((i+1)%4==0){
            score = new ScoreItem(89);//bar
            score->setPos(temp_x, temp_y);
            addItem(score);
            temp_x += ScoreItem::notespacing;
        }
        if((i+1)%20 == 0){
            temp_y += ScoreItem::linespacing + 10;
            temp_x = ScoreItem::halfnotewidth + 10;
        }
    }
}


uint32_t WavScene::load(QString fileName, uint32_t _off, uint32_t _len){
    std::fstream wavin;
    wavin.open(fileName.toStdString().c_str(),std::ios::in|std::ios::binary);
    if(!wavin)
        return 0;
    else
    {
        uint32_t len0 = 0;
        wavin.seekp(40,std::ios::beg);
        wavin.read((char*)&len0, 4);
        qDebug()<<"the wave file length: "<<len0;
        if(_off + _len<len0){
            len0 = _len;
        }else{
            _off = 0;
        }
        ldata = new short[len0];
        rdata = new short[len0];
        wavin.seekp(44+_off,std::ios::beg);
        for(uint32_t i=0;i<len0;++i){
            wavin.read((char*)(ldata+i),2);
            wavin.read((char*)(rdata+i),2);
        }
        wavin.close();
        qDebug()<<"load successful: "<<len0;
        return len0;
    }
}

uint32_t ScoreScene::load(QString fileName){
    std::fstream scorein;
    scorein.open(fileName.toStdString().c_str(), std::ios::in);

    if(!scorein)
        return 0;
    else{
        int tmp;
        while(!scorein.eof()){
            scorein>>tmp;
            note.push_back(uchar(tmp+48));
            scorein>>tmp;
            dura.push_back(uint8_t(tmp));
        }
        scorein.close();
        return note.size();
    }

}

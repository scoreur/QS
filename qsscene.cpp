#include "qsscene.h"
#include "pianokey.h"
#include "scoreitem.h"
#include "wavchannel.h"
#include <QGraphicsView>
#include <QGraphicsItem>
#include <fstream>
#include <QFile>
#include <QDebug>

/// @brief constructor of the keyboard
KeyScene::KeyScene(QGraphicsView *view, QObject *parent)
    : QGraphicsScene(view->geometry(), parent),
      board(new QGraphicsRectItem(0,0, 104*PianoKey::halfspacing,PianoKey::whiteheight))
{
    view->setScene(this);
    //setItemIndexMethod(NoIndex);//default
    view->setRenderHint(QPainter::Antialiasing);
    view->setUpdatesEnabled(true);

    qreal temp_x = PianoKey::halfspacing, temp_y = view->y()+2;
    addItem(board);
    for(uchar i=0;i<88;++i){
        (new PianoKey(i,board))->setPos(temp_x, temp_y);
        if(i%12==2 || i%12 ==7) //B, E
            temp_x += PianoKey::halfspacing;
        temp_x += PianoKey::halfspacing;
    }
}
KeyScene::~KeyScene(){
    delete board;
    qDebug()<<"keyScene removed!";
}

/// @brief constructor of base class QSScene
QSScene::QSScene(QGraphicsView *view, QString fileName):
        QGraphicsScene(view->geometry(),view), name(fileName),
    act (new QAction(QString("QS~%1").arg(fileName.right(16)), this))
{
        view->setScene(this);
        view->setRenderHint(QPainter::Antialiasing);
        act->setCheckable(true);
}
QSScene::~QSScene(){
    delete act;
}
void QSScene::setName(QString fileName){
    name = fileName;
    act->setText(QString("QS~%1").arg(fileName.right(16)));
}

qreal WavScene::secondsPerView = 0.05;
quint32 WavScene::wavBufferSize = 80000;

/// @brief constructor of WavScene
WavScene::WavScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName), ldata(new short[1]), rdata(new short[1]),
      offset(0), len(wavBufferSize)

{
    setItemIndexMethod(NoIndex);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //view->setCacheMode(QGraphicsView::CacheBackground);
    //view->setSceneRect(sceneRect());
    setBackgroundBrush(QColor(0,150,0,100));

    if(fileName != "" && load(fileName, offset, wavBufferSize) != 0){
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

        setSceneRect(0,0, channel->boundingRect().width(),temp_h);

    }
    if(fileName.isEmpty())
        setName("Untitled.wav");
    addRect(sceneRect(), QPen(Qt::black));
    addLine(0,0.50*height(), width(),0.50*height(), QPen(Qt::darkGreen));
    addLine(0,0.25*height(), width(),0.25*height(), QPen(Qt::darkGreen));
    addLine(0,0.75*height(), width(),0.75*height(), QPen(Qt::darkGreen));
}
WavScene::~WavScene(){
    delete []ldata; delete []rdata;
    qDebug()<<"wavScene removed!";
}

/// @brief constructor of ScoreScene
ScoreScene::ScoreScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName)
{
    setItemIndexMethod(BspTreeIndex);

    if(fileName != ""){
        len = load(fileName);
    }else{
        len = 0;
        setName("Untitled.txt");
    }

    if(len == 0){
        len = 88;
        for(quint32 i=0;i<len;++i){
            note.push_back(i);
            dura.push_back(6);
        }
    }
    qreal temp_x = 10, temp_y = ScoreItem::halfnoteheight + 10;
    int totaldura = 0;
    ScoreItem *score;
    for(quint32 i=0;i<len;++i){//typesetting

        score = new ScoreItem(note[i], dura[i]);
        temp_x += (ScoreItem::notespacing * dura[i]/12.0);//add x before add score
        score->setPos(temp_x, temp_y);
        addItem(score);
        totaldura += dura[i];
        if((totaldura) % 48==0){
            temp_x += ScoreItem::notespacing * 0.75;
            score = new ScoreItem(89);//bar
            score->setPos(temp_x, temp_y);
            addItem(score);

        }

        if((totaldura) % 192 == 0){//4 bars per line
            temp_y += ScoreItem::linespacing;
            temp_x = 10;
        }
    }
}
ScoreScene::~ScoreScene(){
    qDebug()<<"scoreScene removed!";
}

/// @brief constructor of StaffScene
StaffScene::StaffScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName)
{
    if(fileName.isEmpty())
        setName("Untitled.mid");

}
StaffScene::~StaffScene(){
    qDebug()<<"staffScene removed!";
}

////////////////////////////////////////////////////////////////

quint32 WavScene::load(QString fileName, quint32 _off, quint32 _len){
    QFile filein(fileName, this);
    filein.open(QFileDevice::ReadOnly);
    if(filein.isOpen()){
        qDebug()<<"file can be open!";
        filein.close();
    }
    std::fstream wavin;
    wavin.open(fileName.toStdString().c_str(),std::ios::in|std::ios::binary);
    if(!wavin)
        return 0;
    else
    {
        quint32 len0 = 0;
        wavin.seekg(16,std::ios::beg);
        wavin.read((char*)&len0, 4);//get 16 or 18
        len0 += 24;
        wavin.seekg(len0,std::ios::beg);
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
        wavin.seekp(44+_off,std::ios::beg);
        for(quint32 i=0;i<len0;++i){
            wavin.read((char*)(ldata+i),2);
            wavin.read((char*)(rdata+i),2);
        }
        wavin.close();
        qDebug()<<"load successful: "<<len0;
        return len = len0;
    }
}

quint32 ScoreScene::load(QString fileName){
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
            dura.push_back(quint8(tmp));
        }
        scorein.close();
        return note.size();
    }

}

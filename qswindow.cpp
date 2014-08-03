#include "qswindow.h"
#include "ui_qswindow.h"
#include "scoreitem.h"
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <fstream>

qreal QSWindow::wavViewSec = 0.5;
QSWindow::QSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QSWindow),
    openFileName(""),
    saveFileName("")
{
    //initialize
    ui->setupUi(this);
    keyBoardInit();
    wavFormInit();
    scoreInit();
scoreUpdate();//test

    //enhancing effect
    ui->statusBar->showMessage("Welcome to QtScoreur!");


    //connettion:
    connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionSave_File, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->actionSave_File_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(ui->actionShow_wave_form, SIGNAL(triggered()), this, SLOT(wavFormUpdate()));

}

QSWindow::~QSWindow()
{
    delete keyScene;
    delete wavScene;
    delete scoreScene;
    delete ui;

}

void QSWindow::keyBoardInit(){
    keyScene = new QGraphicsScene(this);
    keyScene->setSceneRect(ui->keyView->geometry()),
    ui->keyView->setScene(keyScene);

    ui->keyView->setRenderHint(QPainter::Antialiasing);
    keyScene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
    qreal temp_x = PianoKey::halfspacing, temp_y = ui->keyView->y()+2;
    for(uchar i=0;i<88;++i){
        PianoKey *pianoKey = new PianoKey(i);
        pianoKey->setPos(temp_x, temp_y);
        keyScene->addItem(pianoKey);
        if(i%12==2 || i%12 ==7) //B, E
            temp_x += PianoKey::halfspacing;
        temp_x += PianoKey::halfspacing;
    }
}
void QSWindow::wavFormInit(){
    wavScene = new QGraphicsScene(this);
    wavScene->setSceneRect(ui->wavView->geometry());
    ui->wavView->setScene(wavScene);
    ui->wavView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->wavView->setCacheMode(QGraphicsView::CacheBackground);
    wavScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->wavView->setRenderHint(QPainter::Antialiasing);
}
void QSWindow::scoreInit(){
    scoreScene = new QGraphicsScene(this);
    scoreScene->setSceneRect(ui->scoreView->geometry()),
    ui->scoreView->setScene(scoreScene);
    ui->scoreView->setRenderHint(QPainter::Antialiasing);
    scoreScene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);

}

//file control module
void QSWindow::openFile(){
    if(ui->tabWidget->currentIndex()==0)
        openFileName = QFileDialog::getOpenFileName(this, "Open .wav File: ", QDir::currentPath(), "*.wav");
    else
        openFileName = QFileDialog::getOpenFileName(this, "Open .txt File: ", QDir::currentPath(), "*.txt");

    if(openFileName.isEmpty())
        ui->statusBar->showMessage("open file unchosen!", 5000);
    else{
        ui->statusBar->showMessage(QString("open: %1; save: %2").arg(openFileName).arg(saveFileName));
        if(ui->tabWidget->currentIndex()==0)
            wavFormUpdate();
        else
        {

            scoreUpdate();
            //qDebug()<<"update";
        }

    }

}
void QSWindow::saveFile(){
    if(ui->tabWidget->currentIndex()==1)
        saveFileName = QFileDialog::getSaveFileName(this, "Save .wav File: ", QDir::currentPath(), "*.wav");
    else
        saveFileName = QFileDialog::getSaveFileName(this, "Save .txt File: ", QDir::currentPath(), "*.txt");

    if(saveFileName.isEmpty())
        ui->statusBar->showMessage("save file unchosen!", 5000);
    else
    {
        if(QDir(saveFileName).exists()){
            ui->statusBar->showMessage(QString("the file %1 already exists! Please select another!").arg(saveFileName),5000);
            saveFileName = "";
        }
        else{
            ui->statusBar->showMessage(QString("open: %1; save: %2").arg(openFileName).arg(saveFileName));
        }

    }
}
void QSWindow::saveFileAs(){

}
// *********

void QSWindow::on_verticalScrollBar_valueChanged(int value)
{
    qDebug()<<value;
}


// scene update
void QSWindow::wavFormUpdate(){
    wavdatasize = wavLoad();
    if(wavdatasize != 0){
        qreal temp_h = (qreal)ui->wavView->height();
        qreal temp_w = (qreal)ui->wavView->width();
        wavScene->clear();
        WavFormChannel * channel = new WavFormChannel(wavdatasize, wavldata,
        temp_w/44100.0/wavViewSec, 0.24*temp_h);
        channel->setPos(0,0.25 * temp_h);
        wavScene->setSceneRect(0,0, channel->boundingRect().width(),temp_h);
        wavScene->addItem(channel);
        channel = new WavFormChannel(wavdatasize, wavrdata,
            temp_w/44100.0/wavViewSec, 0.24*temp_h);
        channel->setPos(0,0.75 * temp_h);
        wavScene->addItem(channel);
    }

    else{
        ui->statusBar->showMessage("fail to update the wave form!", 5000);
    }
}

void QSWindow::scoreUpdate(){
    scoreScene->clear();
    ScoreItem *score;
    qreal temp_x = ScoreItem::halfnotewidth + 10, temp_y = ScoreItem::halfnoteheight + 10;
    for(int i=0;i<89;++i){//test

        score = new ScoreItem(i);

        score->setPos(temp_x, temp_y);
        scoreScene->addItem(score);

        temp_x += ScoreItem::notespacing;

        if((i+1)%4==0){
            score = new ScoreItem(89);//bar
            score->setPos(temp_x, temp_y);
            scoreScene->addItem(score);
            temp_x += ScoreItem::notespacing;
        }
        if((i+1)%20 == 0){
            temp_y += ScoreItem::linespacing + 10;
            temp_x = ScoreItem::halfnotewidth + 10;
        }


    }
        ;
}

qulonglong QSWindow::wavLoad(){
    std::fstream wavin;
    wavin.open(openFileName.toStdString().c_str(),std::ios::in|std::ios::binary);

    if(!wavin)
        return 0;
    else
    {
        ulong len = 0;
        wavin.seekp(10040,std::ios::beg);
        wavin.read((char*)&len, 4);
        qDebug()<<len;
        len = 100000;//debug
        if(wavldata != 0) delete []wavldata;
        if(wavrdata != 0) delete []wavrdata;
        wavldata = new short[len];
        wavrdata = new short[len];

        for(ulong i=0;i<len;++i){
            wavin.read((char*)(wavldata+i),2);
            wavin.read((char*)(wavrdata+i),2);
        }
        wavin.close();
        //qDebug()<<"load successful: "<<len;
        return len;
    }
}
// *********

//relevant classes definition

WavFormChannel::WavFormChannel(qulonglong _datasize, short *_data, qreal _intv, qreal _amp):
    datasize(_datasize), interval(_intv), amplitude(_amp),
    bound(QRectF(0,-_amp, _intv*_datasize,_amp*2)){
    data = new QPointF[datasize];
    qreal temp_x = 0;
    for(qulonglong i=0;i<datasize;++i){
        data[i].setX(temp_x);
        data[i].setY(-amplitude /(1<<15)*_data[i]);//正负2的15次幂
        temp_x += interval;
    }
    //qDebug()<<"construct channel: "<<temp_x;
}

void WavFormChannel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen pen0(Qt::black);
    painter->setPen(pen0);
    painter->drawLine(QPointF(bound.left(),0), QPointF(bound.right(),0));

    QPen pen(Qt::green);

    painter->setPen(pen);

    if(datasize>0)
        painter->drawPolyline(data, datasize);

}

QRectF WavFormChannel::boundingRect() const{
    return bound;
}
QPainterPath WavFormChannel::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

#include "qsscene.h"
#include "pianokey.h"
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDir>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QDebug>
#include "Wave/wavframe.h"
#include "qswindow.h"


/// @brief constructor of the keyboard
KeyScene::KeyScene(QGraphicsView *view, QWidget *parent)
    : QGraphicsScene(view->geometry(), parent),
      board(new QGraphicsRectItem(0,0, 104*PianoKey::halfspacing,PianoKey::whiteheight))

{
    view->setScene(this);
    //setItemIndexMethod(NoIndex);//default
    view->setRenderHint(QPainter::Antialiasing);
    view->setUpdatesEnabled(true);
    keyTime.start();


    qreal temp_x = PianoKey::halfspacing, temp_y = view->y()+2;
    addItem(board);
    for(uchar i=0;i<88;++i){

        (new PianoKey(i,board))->setPos(temp_x, temp_y);
        spectrum[i] = QPointF(temp_x, 0);
        if(i%12==2 || i%12 ==7) //B, E
            temp_x += PianoKey::halfspacing;
        temp_x += PianoKey::halfspacing;
    }
#ifndef HAVE_NATURAL_KEYSOUND
    if(QDir::home().mkpath(QString("QS_tmp/keysound"))){
        qDebug()<<"create:"<<WavFile::keysoundGen(QDir::homePath().append("/QS_tmp/keysound"), 1.2);
    }else qDebug()<<"keysoundGen error!";
#endif



}
void KeyScene::drawForeground(QPainter *painter, const QRectF &rect){
    Q_UNUSED(rect)
    QPen pen(Qt::red);
    painter->setPen(pen);
    painter->drawPolyline(spectrum, 88);


}

KeyScene::~KeyScene(){
    delete board;

    qDebug()<<"keyScene removed!";
}
void KeyScene::keyInput(quint8 id){

    ((QSWindow*)parent())->keyInput(id, (quint8)(keyTime.elapsed()/PianoKey::keyTimeAccuracy));
}

quint8 KeyScene::PianoKeyCode[13] = {'A','W','S','E','D','F','U','J','I','K','O','L','P'};
qint8 KeyScene::keyhold = 0;
void KeyScene::keyPressEvent(QKeyEvent *event){
    if(event->key()=='Q'){
        if(PianoKey::scope>=12 && keyhold==0){
            PianoKey::scope-=12;

            // TODO: add ui effect

        }

        return;
    }
    if(event->key()==0x3b){//semi-colon
        //qDebug()<<"keyhold: "<<keyhold;
        if(PianoKey::scope<=63  && keyhold==0){

            PianoKey::scope+=12;
            // TODO: add ui effect
        }

        return;
    }
    if(event->key()=='G'){
        if(PianoKey::scope<=74 && keyhold==0){
            ++PianoKey::scope;

            // TODO: add ui effect

        }

        return;
    }
    if(event->key()=='H'){
        if(PianoKey::scope>=1 && keyhold==0){
            --PianoKey::scope;

            // TODO: add ui effect

        }

        return;
    }
    quint16 i=0;
    while(i<13 && PianoKeyCode[i]!=event->key()) ++i;
    if(i>=13)
        return;
    else{
        i+=PianoKey::scope;
        if(((PianoKey*)board->childItems()[i])->is_pressed)
            return;
        ((PianoKey*)board->childItems()[i])->keyPress();
        ++keyhold;
    }
}
void KeyScene::keyReleaseEvent(QKeyEvent *event){
    quint16 i=0;
    while(i<13 && PianoKeyCode[i]!=event->key()) ++i;
    if(i>=13)
        return;
    ((PianoKey*)board->childItems()[PianoKey::scope+i])->keyRelease();
    if(keyhold>0) --keyhold;


}



/// @brief constructor of base class QSScene
QSScene::QSScene(QGraphicsView *view, const QString &fileName):
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






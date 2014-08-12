#include "qsscene.h"
#include "pianokey.h"
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDebug>
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
void KeyScene::keyInput(quint8 id){
    ((QSWindow*)parent())->keyInput(id);
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





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






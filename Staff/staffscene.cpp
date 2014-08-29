#include "staffscene.h"
#include "qspreset.h"
#include <QDebug>
#include <QPicture>

/// @brief constructor of StaffScene
StaffScene::StaffScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName)
{
    if(fileName.isEmpty())
        setName("Untitled.mid");
    setBackgroundBrush(QBrush(QPixmap(QString(":/image/staff/paper2.png"), "PNG").scaled(100,100)));

    StaffLine * staffLine = new StaffLine(this, LineType::NOTES);
    staffLine->setPos(20,20);
    lines.push_back(staffLine);
    new KeySignature(staffLine, 1, -5);
    qreal startX = 100, startY = 10, intv = QSPreset::staffLineInterval;
    for(quint16 i=0;i<10;++i)
        (new StaffItem(staffLine, 24-(i*12)%24))->setPos(startX+intv*2*i, startY + i*intv/2.0);
    //quint8 test_notes[8] = {39,41,43,44,46,48,50,51};


}
StaffScene::~StaffScene(){

    qDebug()<<"staffScene removed!";
}



quint32 StaffScene::load(QString fileName){
    return fileName.size();
}
quint32 StaffScene::store(QString fileName){
    return fileName.size();
}

StaffMeasure::StaffMeasure(QGraphicsItem *parent):
    QGraphicsRectItem(parent)
{
    ;
}

/////////////////////////////////////////////////////////////
/// @brief constructor of StaffLine
///
/// holding all notes in one track or display title/lyrics/etc.
///////////////////////////////////////////////////////////////
StaffLine::StaffLine(QGraphicsScene *scene, quint32 _type, QGraphicsItem *parent) :
    QGraphicsRectItem(parent), type((LineType)_type){
    scene->addItem(this);
    setRect(-20,-20, QSPreset::staffLineSize.width(), QSPreset::staffLineSize.height());



}
void StaffLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPicture pic;
    pic.load("");

    QPen pen(Qt::black);
    painter->setPen(pen);
    QPointF startPos = QPointF(0,10);
    qreal endX = QSPreset::staffPaddingRect.right();
    for(int i=0; i<5; ++i){// draw five lines
        painter->drawLine(startPos.x(),startPos.y()+i*QSPreset::staffLineInterval,
                          endX,startPos.y()+i*QSPreset::staffLineInterval);
    }



}

void StaffLine::mousePressEvent(QGraphicsSceneMouseEvent *event){// TODO:
    Q_UNUSED(event)
}

void StaffLine::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
}

void StaffLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
}


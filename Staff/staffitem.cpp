#include "staffitem.h"
#include "../qspreset.h"
#include "../qsscene.h"
////////////////////
/// \brief Constructor of StaffItem
/// \param parent
/// \param _dura
///
StaffItem::StaffItem(QGraphicsItem *parent, quint8 _dura) :
    QGraphicsObject(parent), dura(_dura), colorIndex(0),
    markShowed(false), bound(-5,-5,10,10)
{

}

void StaffItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(dura == 24)
        painter->drawPicture(0,0,QSPreset::getInstance()->imgs(StaffImages::half_note_head));
    else if(dura == 12)
        painter->drawPicture(0,0,QSPreset::getInstance()->imgs(StaffImages::qut_note_head));
    else if(dura == 6)
        ;
}

void StaffItem::showMark(bool f){
    markShowed = f;
}




QPainterPath StaffItem::shape() const{
    QPainterPath tmp;
    tmp.addRect(bound);
    return tmp;
}

QRectF StaffItem::boundingRect() const{
    return bound;
}

void StaffItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
}

void StaffItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
}

void StaffItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    setPos(event->scenePos());
    scene()->update(sceneBoundingRect());
}

void StaffItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
}

//////////////////////////////////////////
/// \brief constructor of KeySignature
///
/// \param parent
/// \param _clef
/// \param _id
///
KeySignature::KeySignature(QGraphicsItem *parent, qint8 _clef, qint8 _id) :
    QGraphicsObject(parent), clef(_clef), id(_id), bound(QRectF(0,0, 100,100)),
    clefImg(_clef==0? QSPreset::getInstance()->imgs[StaffImages::tr_clef]:
                      QSPreset::getInstance()->imgs[StaffImages::ba_clef]),
    markImg(_id>=0? QSPreset::getInstance()->imgs[StaffImages::sharp]:
                    QSPreset::getInstance()->imgs[StaffImages::flat]),
    clefPos(_clef==0? QPointF(-20,-8) : QPointF(-12,4))
{
    constructPlaces();
}
void KeySignature::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->drawPixmap(clefPos, clefImg);
    for(QVector<QPointF>::iterator iter = places.begin(); iter != places.end(); ++iter)
        painter->drawPixmap(*iter, markImg);
}
QPainterPath KeySignature::shape() const{
    QPainterPath tmp;
    tmp.addRect(bound);
    return tmp;
}

void KeySignature::constructPlaces(){
    if(! places.empty())
        places.clear();
    if(clef == 0){
        if(id > 0){//sharp key
            for(qint8 i=0; i<id; ++i){
                places << QPointF(i*7+20,(i*3)%5 * QSPreset::staffLineInterval/2-3);
            }
        }
        else if(id < 0)//flat key
            for(quint8 i=0; i<-id; ++i){
                places<< QPointF(i*7+20,(2 - (i*3)%5) * QSPreset::staffLineInterval/2+2);
            }
    }else if(clef == 1){
        if(id > 0){//sharp key
            for(qint8 i=0; i<id; ++i){
                places << QPointF(i*7+20,(2 + (i*3)%5) * QSPreset::staffLineInterval/2-3);
            }
        }
        else if(id < 0)//flat key
            for(quint8 i=0; i<-id; ++i){
                places<< QPointF(i*7+20,(4 - (i*3)%5) * QSPreset::staffLineInterval/2+2);
            }

    }else{

    }

}

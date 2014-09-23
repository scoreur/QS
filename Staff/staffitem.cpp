#include "staffitem.h"
#include "qspreset.h"
#include "qsscene.h"
#include <QDir>
#include <QDebug>
////////////////////
/// \brief Constructor of StaffItem
/// \param parent
/// \param _dura
///
StaffItem::StaffItem(QGraphicsItem *parent, quint8 _dura) :
    QGraphicsObject(parent), dura(_dura), colorIndex(0),
    markShowed(false), isPressed(false), bound(-30,-30,20,20)
{

}

void StaffItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(isPressed){
        QBrush brush(QColor(0,200,50,50));
        painter->setPen(Qt::NoPen);
        painter->setBrush(brush);
        painter->drawRect(bound);
        //qDebug()<<"drawRect!";

    }
    QPicture picture;
    if(dura == 48){
        painter->setBrush(Qt::NoBrush);
        painter->setPen(Qt::black);
        drawFullNote(painter, QPointF(-20,-10));
    }else if(dura >= 24){

        picture.load(QDir::homePath().append("/black_solid_note.pic"));
        painter->drawPicture(-20,-20,picture);

    }
    else{

        picture.load(QDir::homePath().append("/black_hollow_note.pic"));
        painter->drawPicture(-20,-20, picture);
    }




}

void StaffItem::showMark(bool f){
    markShowed = f;
    scene()->update(sceneBoundingRect());
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
    isPressed = true;
    //qDebug()<<"pressed!";
    mouseY = mapToParent(event->pos()).y();
    scene()->update(sceneBoundingRect());

}

void StaffItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
    isPressed = false;
    mouseY = 0;
    scene()->update(sceneBoundingRect());
}

void StaffItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    //if(mapToParent(pos()).y()<mouseY+30 && mapToParent(pos()).y()>mouseY-30){
        setY(mapToParent(event->pos()).y()-boundingRect().top()-10);
        scene()->update(sceneBoundingRect());
    //}
}

void StaffItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
}

qreal StaffItem::mouseY = 0;//for mouse event

//////////////////////////////////////////
/// \brief constructor of KeySignature
///
/// \param parent
/// \param _clef
/// \param _id
///
KeySignature::KeySignature(QGraphicsItem *parent, qint8 _clef, qint8 _id) :
    QGraphicsObject(parent), clef(_clef), id(_id), bound(QRectF(0,0, 100,100)),
    clefImg(_clef==0? QSPreset::getInstance()->images[QSPreset::tr_clef]:
                      QSPreset::getInstance()->images[QSPreset::ba_clef]),
    markImg(_id>=0? QSPreset::getInstance()->images[QSPreset::sharp]:
                    QSPreset::getInstance()->images[QSPreset::flat]),
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
    qreal startX = 20, intv = QSPreset::staffLineInterval/2;
    if(clef == 0){//tremble clef
        if(id > 0){//sharp key
            for(qint8 i=0; i<id; ++i){
                places << QPointF(i*7+startX,(i*3)%5 * intv - 3);
            }
        }
        else if(id < 0)//flat key
            for(quint8 i=0; i<-id; ++i){
                places<< QPointF(i*7+startX,(2 - (i*3)%5) * intv+2);
            }
    }else if(clef == 1){//bass clef
        if(id > 0){//sharp key
            for(qint8 i=0; i<id; ++i){
                places << QPointF(i*7+startX,(2 + (i*3)%5) * intv-3);
            }
        }
        else if(id < 0)//flat key
            for(quint8 i=0; i<-id; ++i){
                places<< QPointF(i*7+startX,(4 - (i*3)%5) * intv+2);
            }

    }else{

    }

}

////////////////////
//present painter commands which would be in QSPreset.cpp, for testing
void drawNoteHead(QPainter *painter, QPointF center){
    //QPen pen(QSPreset::noteColor[0]); QBrush brush(QSPreset::noteColor[0]);
    qreal shx = 0, shy = -0.35; qreal intv = QSPreset::staffLineInterval;
    //painter->setPen(pen);
    //painter->setBrush(brush);
    painter->shear(shx,shy);
    painter->drawEllipse(center, intv *0.65, intv*0.45);
    painter->shear(-shx, -shy);
    painter->setTransform(QTransform());
}
void drawFullNote(QPainter *painter, QPointF center){
    qreal shx = 0, shy = 0.25; qreal intv = QSPreset::staffLineInterval;
    painter->shear(shx,shy);
    painter->drawEllipse(center, intv *0.65, intv*0.6);
    //painter->shear(shy, shx);
    painter->setTransform(QTransform());
}



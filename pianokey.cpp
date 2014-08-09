#include "pianokey.h"
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QCursor>




/// @brief constructor of piano key
PianoKey::PianoKey(uchar _id, QGraphicsItem *parent):
    QGraphicsItem(parent),
    id(_id),is_white(isWhite(_id)),
    path(defaultKeyShape(_id)),
    bound(
         is_white?
         QRectF(-halfspacing,0, halfspacing*2,whiteheight):
         QRectF(-halfwidth,0, halfwidth*2,blackheight)
         ),
    is_pressed(false)
{
    setToolTip(QString("I'm No.%1 from the left!").arg(id+1));
    //qDebug()<< id <<"success";
}

/// @brief Mouse Event
void PianoKey::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        press(true);

    }
}

void PianoKey::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        press(false);
    }
}

void PianoKey::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if(toolTip().right(5) == "left!")
            setToolTip(QString("I'm No.%1 from the right!").arg(88-id));
        else
            setToolTip(QString("I'm No.%1 from the left!").arg(id+1));
    }
}


void PianoKey::press(bool f){
    if(f != is_pressed){
        is_pressed = f;
        scene()->update(sceneBoundingRect());//important!
    }// to be continued to add sound
}

QColor PianoKey::color(){
    if(is_pressed){
        if(is_white)
            return QColor(0xeeeeee);
        else
            return QColor(0x666666);
    }
    else if(is_white){
        return Qt::white;
    }
    else
        return Qt::black;
}

void PianoKey::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QBrush brush(color());
    QPen pen(Qt::darkGray);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setBrush(brush);
    painter->setPen(pen);
    painter->drawPath(path);
}

//static member initializing
qreal PianoKey::halfspacing = 9.0;
qreal PianoKey::halfwidth = 6.0;
qreal PianoKey::whiteheight = 120.0;
qreal PianoKey::blackheight = 65.0;

QRectF PianoKey::defaultKeyBound(uchar _id){//currently unused!
    qreal temp_x = 0;
    for(uchar i=0;i<_id;++i){
        if(i%12==2 || i%12 ==7) //B, E
            temp_x += PianoKey::halfspacing;
        temp_x += PianoKey::halfspacing;
    }
    return isWhite(_id)?
                QRectF(temp_x,0, halfspacing*2,whiteheight):
                QRectF(temp_x,0, halfwidth*2,blackheight);
}

QPainterPath PianoKey::defaultKeyShape(uchar _id){//y=0 set on upside of keyboard
    TYPE type = TYPE(0);
    if(_id==0) //leftmost key
        type = WHITE_LEFT;
    else if(_id==87) //rightmost key
        type = WHITE_WHOLE;
    else
        switch(_id % 12){
        case 1: case 4: case 6: case 9: case 11:
            type = BLACK; break;
        case 3: case 8: //C, F
            type = WHITE_LEFT; break;
        case 0: case 5: case 10: //A, D, G
            type = WHITE_NORMAL; break;
        case 2:case 7:  //B, E
            type = WHITE_RIGHT; break;
        default: break;
        }
    QPainterPath temp;
    QPolygonF polygon;
    switch(type){
    case BLACK:
/*        polygon<<QPointF(-halfwidth,0)
               <<QPointF(halfwidth,0)
               <<QPointF(halfwidth,blackheight)
               <<QPointF(-halfwidth,blackheight)
               <<QPointF(-halfwidth, 0); */
        temp.addRoundRect(QRectF(-halfwidth,0, 2*halfwidth, blackheight), 70);//roundness 70
        return temp;
               break;
    case WHITE_LEFT:
        polygon<<QPointF(-halfspacing,0)
               <<QPointF(halfspacing-halfwidth, 0)
               <<QPointF(halfspacing-halfwidth, blackheight)
               <<QPointF(halfspacing, blackheight)
               <<QPointF(halfspacing, whiteheight)
               <<QPointF(-halfspacing, whiteheight)
               <<QPointF(-halfspacing, 0);
               break;
    case WHITE_NORMAL:
        polygon<<QPointF(halfwidth-halfspacing, 0)
               <<QPointF(halfspacing-halfwidth, 0)
               <<QPointF(halfspacing-halfwidth, blackheight)
               <<QPointF(halfspacing, blackheight)
               <<QPointF(halfspacing, whiteheight)
               <<QPointF(-halfspacing, whiteheight)
               <<QPointF(-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, 0);
               break;
    case WHITE_RIGHT:
        polygon<<QPointF(halfwidth-halfspacing, 0)
               <<QPointF(halfspacing, 0)
               <<QPointF(halfspacing, whiteheight)
               <<QPointF(-halfspacing, whiteheight)
               <<QPointF(-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, 0);
               break;
    case WHITE_WHOLE:
        polygon<<QPointF(-halfspacing,0)
               <<QPointF(halfspacing,0)
               <<QPointF(halfspacing,whiteheight)
               <<QPointF(-halfspacing,whiteheight)
               <<QPointF(-halfspacing, 0);
               break;


    default: break;
    }

    temp.addPolygon(polygon);
    return temp;

}

bool PianoKey::isWhite(uchar _id){
    switch(_id % 12){
    case 1: case 4: case 6: case 9: case 11:
    //black keys
        return false;
    default:
        return true;
    }
}




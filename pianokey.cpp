#include "pianokey.h"
#include <QDebug>

//static member initializing
qreal PianoKey::halfspacing = 9.0;
qreal PianoKey::halfwidth = 6.0;
qreal PianoKey::whiteheight = 120.0;
qreal PianoKey::blackheight = 65.0;

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
        polygon<<QPointF(-halfwidth,0)
               <<QPointF(halfwidth,0)
               <<QPointF(halfwidth,blackheight)
               <<QPointF(-halfwidth,blackheight)
               <<QPointF(-halfwidth, 0);
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

//static member initializing finished


PianoKey::PianoKey(uchar _id):
    id(_id),
    path(defaultKeyShape(_id)),
    bound(
         isWhite(_id)?
         QRectF(-halfspacing,0, halfspacing*2,whiteheight):
         QRectF(-halfwidth,0, halfwidth*2,blackheight)
         ),
    isPressed(false)
{
    //qDebug()<< id <<"success";
}

void PianoKey::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QBrush brush(color());
    painter->setBrush(brush);
    painter->drawPath(path);
}

QRectF PianoKey::boundingRect() const{
    return bound;
}

QPainterPath PianoKey::shape() const{
    return path;
}

QColor PianoKey::color(){
    if(isPressed)
        return Qt::gray;
    else if(isWhite(id))
        return Qt::white;
    else
        return Qt::black;
}

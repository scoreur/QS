#include "scoreitem.h"
#include <QDebug>
// static member initailizing
qint8 ScoreItem::keyPitch = -9;//C major, A as 0
qint8 ScoreItem::halfnotewidth = 14;
qint8 ScoreItem::halfnoteheight = 25;
qint8 ScoreItem::notespacing = 25;
qint8 ScoreItem::linespacing = 50;
qint8 ScoreItem::notesize = 20;

// *******


ScoreItem::ScoreItem(uchar _pitch, uchar _duration):
    pitch(_pitch), duration(_duration), oct(0),
    bound(QRectF(-halfnotewidth,-halfnoteheight,
          halfnotewidth*2,halfnoteheight*2))
{  
    noteUpdate();
}

void ScoreItem::noteUpdate(qint8 adjust){

    pitch += adjust;
    switch((pitch - keyPitch)%12){
    case 0: note = '1'; semi = 0;
        break;
    case 1: note = '1'; semi = 1;
        break;
    case 2: note = '2'; semi = 0;
        break;
    case 3: note = '3'; semi = -1;
        break;
    case 4: note = '3'; semi = 0;
        break;
    case 5: note = '4'; semi = 0;
        break;
    case 6: note = '4'; semi = 1;
        break;
    case 7: note = '5'; semi = 0;
        break;
    case 8: note = '5'; semi = 1;
        break;
    case 9: note = '6'; semi = 0;
        break;
    case 10: note = '7'; semi = -1;
        break;
    case 11: note = '7'; semi = 0;
        break;
    default: break;
    }
    if(pitch == 88)
        note = '0';
    else
        oct = ((pitch-keyPitch)/12-4);
    //qDebug()<<int((pitch-keyPitch)%12);
}


void ScoreItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen(Qt::black);
    painter->setPen(pen);

    if(pitch<89){
        QFont font("Times [Adobe]", notesize , QFont::Bold);
        painter->setFont(font);
        painter->drawText(bound, Qt::AlignCenter, QString(note));

        if(oct != 0){
            qint8 direc = oct>0? 1:-1;
            for(qint8 i=1;i<=oct*direc;++i){
                QBrush brush(Qt::black);
                painter->setBrush(brush);
                painter->drawEllipse(0,direc*(-notesize/2-i*3),1,1);
            }
        }
    }else switch(pitch){
    case 89:
        painter->drawLine(0,-notesize *0.75, 0,notesize*0.75);
        break;
    case 90://undefined
    default: break;
    }


}

QRectF ScoreItem::boundingRect() const{
    return bound;
}
QPainterPath ScoreItem::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}


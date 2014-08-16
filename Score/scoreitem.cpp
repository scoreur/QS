#include "scoreitem.h"
#include <QGraphicsItem>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>



/// @brief constructor of score notation
ScoreItem::ScoreItem(uchar _pitch, uchar _duration, QGraphicsItem *parent):
    QGraphicsObject(parent),
    pitch(_pitch), duration(_duration), oct(0),
    bound(QRectF(-halfnotewidth *_duration/12.0,-halfnoteheight,
          halfnotewidth*2 *_duration/12.0,halfnoteheight*2)),
    color(presetColor), mouseY(0)
{  
    setCursor(Qt::ArrowCursor);
    setToolTip("drag to change me");
    noteUpdate();
}

void ScoreItem::noteUpdate(qint8 adjust){
    if(adjust != 0){
        if(pitch > 88) return;
        if(pitch + adjust >88)
            pitch = 88;
        else if( pitch + adjust < 0)
            pitch = 0;
        else
            pitch += adjust;
    }
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
    if(pitch == 88){
        note = '0';
        semi = 0;
    }
    else
        oct = ((pitch-keyPitch)/12-4);
    //qDebug()<<int((pitch-keyPitch)%12);
}


void ScoreItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen(color);
    QBrush brush(color);
    painter->setPen(pen);
    if(pitch<89){       
        QFont font("Times [Adobe]", notesize , QFont::Bold);
        painter->setFont(font);
        painter->drawText(bound, Qt::AlignCenter, QString(note));
        if(duration == 6)//half
            painter->drawLine(-halfnotewidth/2.0,notesize/2.0, halfnotewidth/2.0,notesize/2.0);
        else if(duration == 3){//quarter
            painter->drawLine(-halfnotewidth/2.0,notesize/2.0+2, halfnotewidth/2.0,notesize/2.0+2);
            painter->drawLine(-halfnotewidth/2.0,notesize/2.0, halfnotewidth/2.0,notesize/2.0);
        }else if(duration == 9){//three quarters
            painter->drawLine(-halfnotewidth/2.0,notesize/2.0, halfnotewidth/2.0,notesize/2.0);
            painter->setBrush(brush);
            painter->drawEllipse(halfnotewidth/2.0-2,notesize/2.0-6,1,1);
        }

        if(semi == 1){// #

            font.setPointSizeF(notesize/2);
            painter->setFont(font);
            painter->drawText(-bound.width()/2,-bound.height()/2+8, 10,10, Qt::AlignLeft, QString('#'));
        }
        else if(semi == -1){// b
            font.setPointSizeF(notesize/2);
            painter->setFont(font);
            painter->drawText(-bound.width()/2,-bound.height()/2+8, 10,10, Qt::AlignLeft, QString('b'));
        }
        else
            ;

        if(oct != 0 && pitch != 88){//not rest
            qint8 direc = oct>0? 1:-1;
            for(qint8 i=1;i<=oct*direc;++i){
                painter->setBrush(brush);
                painter->drawEllipse(0,direc*(-notesize/2-i*3),1,1);
            }
        }
        if(mouseY != 0){//mouse event
            painter->setBrush(selectedBrush);
            painter->setPen(Qt::NoPen);
            painter->drawRect(bound);
        }
    }else switch(pitch){
    case 89:
        painter->drawLine(0,-notesize *0.75, 0,notesize*0.75);
        break;
    case 90:
        painter->drawLine(-2,-notesize *0.75, -2,notesize*0.75);
        painter->drawLine(2,-notesize *0.75, 2,notesize*0.75);
    case 91://undefined
        break;

    default: break;
    }


}

QPainterPath ScoreItem::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

// static member initailizing
qint8 ScoreItem::keyPitch = -9;//C major, A as 0
quint8 ScoreItem::halfnotewidth = 18;
quint8 ScoreItem::halfnoteheight = 25;
quint8 ScoreItem::notespacing = 35;
quint8 ScoreItem::linespacing = 50;
quint8 ScoreItem::notesize = 20;
QColor ScoreItem::presetColor = Qt::black;
QBrush ScoreItem::selectedBrush(QColor(0,60,220,20));
// *******

void ScoreItem::scorePreset(int mode, int arg){
    switch(mode){
    case 1:
        if(notespacing != (quint8)arg)
            qDebug()<<"notespacing changed!"<< int(notespacing = (quint8)arg);
        break;
    case 2:
        if(linespacing != (quint8)arg)
            qDebug()<<"linespacing changed!"<< int(linespacing = (quint8)arg);
        break;
    case 3:
        if(notesize != (quint8)arg)
            qDebug()<<"notesize changed!"<< int(notesize = (quint8)arg);
        break;
    default: break;
    }

}




/// @brief mouse events
void ScoreItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        setCursor(Qt::SizeVerCursor);
        color = Qt::red;
        mouseY = event->scenePos().y();
        scene()->update(sceneBoundingRect());

    }

}

void ScoreItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        setCursor(Qt::ArrowCursor);
        color = presetColor;
        mouseY = 0;
        scene()->update(sceneBoundingRect());
    }

}

void ScoreItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    int tmp = (int)(mouseY-event->scenePos().y());
    if(tmp>1)
        noteUpdate(tmp/1-1);
    else if(tmp<-1)
        noteUpdate(1-(-tmp)/1);
    mouseY = event->scenePos().y();
    scene()->update(sceneBoundingRect());
    //qDebug()<<event->scenePos();
}
void ScoreItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    noteUpdate(88-pitch);
    scene()->update(sceneBoundingRect());
    qDebug()<<"double clicked!";
}

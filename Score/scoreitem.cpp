#include "scoreitem.h"
#include "qspreset.h"
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

    note = PitchToNote[(pitch-keyPitch)%12];
    semi = PitchToSemi[(pitch-keyPitch)%12];

    if(pitch == 88){
        note = '0';
        semi = 0;
    }
    else
        oct = ((pitch-keyPitch)/12-4);    
}


void ScoreItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen(color);
    QBrush brush(color);
    QFont font("Times [Adobe]", notesize , QFont::Bold);
    painter->setPen(pen);
    if(pitch<89){       

        painter->setFont(font);
        painter->drawText(bound, Qt::AlignCenter, QString(note));

        if(semi == 1){// #
            painter->drawPixmap(-bound.width()/2-6,-bound.height()/2+5, 20,12,
                                QSPreset::getInstance()->imgs[StaffImages::sharp]);
        }
        else if(semi == -1){// b
            painter->drawPixmap(-bound.width()/2-6,-bound.height()/2+5, 20,12,
                                            QSPreset::getInstance()->imgs[StaffImages::flat]);
        }
        else//nature sign
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
QColor ScoreItem::presetColor = Qt::black;//default color for notes
QBrush ScoreItem::selectedBrush(QColor(0,60,220,20));//the color of note's rect when selected
qint8 ScoreItem::PitchToNote[12] = {49,49,50,51,51,52,52,53,53,54,55,55};//map pitch to note text
qint8 ScoreItem::PitchToSemi[12] = {0,1,0,-1,0,0,1,0,1,0,-1,0};
int ScoreItem::temppitch = 89;
// *******


/// @brief mouse events
void ScoreItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        setCursor(Qt::SizeVerCursor);
        color = Qt::red;
        mouseY = event->scenePos().y();
        temppitch = pitch;
        scene()->update(sceneBoundingRect());

    }

}

void ScoreItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        setCursor(Qt::ArrowCursor);
        color = presetColor;
        mouseY = 0;
        temppitch = 89;
        scene()->update(sceneBoundingRect());
    }

}

void ScoreItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    int tmp = (int)(mouseY-event->scenePos().y());
    if(tmp>5)
        noteUpdate(temppitch-pitch+tmp/5-1);
    else if(tmp<-5)
        noteUpdate(temppitch-pitch+1-(-tmp)/5);

    scene()->update(sceneBoundingRect());
    //qDebug()<<event->scenePos();
}
void ScoreItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)// TODO: replace this macro by USING event
    noteUpdate(88-pitch);
    scene()->update(sceneBoundingRect());
    qDebug()<<"double clicked!";
}

#include "pianokey.h"
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QDir>
#include <QTimer>
#include "General/qsscene.h"



#ifdef HAVE_NATURAL_KEYSOUND
QString PianoKey::soundpath = QString("/Coding/Qt/QS/sound/keyboard/piano_%1.wav");
#else
QString PianoKey::soundpath = QDir::homePath() + QString("/QS_tmp/keysound/piano_%1.wav");
#endif
/// @brief constructor of piano key
PianoKey::PianoKey(uchar _id, QGraphicsItem *parent):
    QGraphicsObject(parent),
    id(_id),is_white(isWhite(_id)),
    path(defaultKeyShape(_id)),
    bound(is_white?whiteBound : blackBound),
    is_pressed(false),
    keysound(new QSound(soundpath.arg(QString::number(id,10),2,'0')))
    //keythread (new QThread(0))
{
    setToolTip(QString("I'm No.%1 from the left!").arg(id+1));

    //keysound->moveToThread(keythread);
}

/// @brief Mouse Event
void PianoKey::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        press(true);
        ((KeyScene*)scene())->keyTime.restart();
        //keythread->start();
        keysound->play();
        //QSound::play(soundpath.arg(QString::number(id,10),2,'0'));

    }
}

void PianoKey::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        press(false);
        ((KeyScene*)scene())->keyInput(id);
        QTimer::singleShot(latency,keysound,SLOT(stop()));//timer cannot function in multithreat
        //keythread->quit();

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

/*
void PianoKey::keyPressEvent(QKeyEvent *event){
    qDebug()<<event->key();
    if(id>=scope && id<=scope+12 && event->key()==PianoKeyCode[id-scope]){
        press(true);
        ((KeyScene*)scene())->keyTime.restart();
        //keythread->start();
        keysound->play();
    }

}

void PianoKey::keyReleaseEvent(QKeyEvent *event){
    if(id>=scope && id<=scope+12 && event->key()==PianoKeyCode[id-scope]){
        press(false);
        ((KeyScene*)scene())->keyInput(id);
        QTimer::singleShot(latency,keysound,SLOT(stop()));//timer cannot function in multithreat
        //keythread->quit();

    }
}
*/
void PianoKey::keyPress(){
    press(true);
    ((KeyScene*)scene())->keyTime.restart();
    keysound->play();
}
void PianoKey::keyRelease(){
    press(false);
    ((KeyScene*)scene())->keyInput(id);

}

quint8 PianoKey::scope = 39;

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

//keyshape parameters
qreal PianoKey::halfspacing = 9.0;
qreal PianoKey::halfwidth = 6.0;
qreal PianoKey::whiteheight = 120.0;
qreal PianoKey::blackheight = 65.0;
QRectF PianoKey::blackBound = QRectF(-halfwidth,0, halfwidth*2,blackheight);
QRectF PianoKey::whiteBound = QRectF(-halfspacing,0, halfspacing*2,whiteheight);
quint16 PianoKey::latency = 300;
quint16 PianoKey::keyTimeAccuracy = 80;

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
/*        polygon<<blackBound.topLeft
               <<blackBound.topRight
               <<QPointF(halfwidth,blackheight)
               <<QPointF(-halfwidth,blackheight)
               <<QPointF(-halfwidth, 0); */// for fine looking
        temp.addRoundRect(blackBound, 70);//roundness 70
        return temp;
               break;
    case WHITE_LEFT:
        polygon<<whiteBound.topLeft()
               <<QPointF(halfspacing-halfwidth, 0)
               <<QPointF(halfspacing-halfwidth, blackheight)
               <<QPointF(halfspacing, blackheight)
               <<whiteBound.bottomRight()
               <<whiteBound.bottomLeft()
               <<whiteBound.topLeft();
               break;
    case WHITE_NORMAL:
        polygon<<QPointF(halfwidth-halfspacing, 0)
               <<QPointF(halfspacing-halfwidth, 0)
               <<QPointF(halfspacing-halfwidth, blackheight)
               <<QPointF(halfspacing, blackheight)
               <<whiteBound.bottomRight()
               <<whiteBound.bottomLeft()
               <<QPointF(-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, 0);
               break;
    case WHITE_RIGHT:
        polygon<<QPointF(halfwidth-halfspacing, 0)
               <<whiteBound.topRight()
               <<whiteBound.bottomRight()
               <<whiteBound.bottomLeft()
               <<QPointF(-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, blackheight)
               <<QPointF(halfwidth-halfspacing, 0);
               break;
    case WHITE_WHOLE:
/*        polygon<<whiteBound.topLeft
               <<whiteBound.topRight
               <<QPointF(halfspacing,whiteheight)
               <<QPointF(-halfspacing,whiteheight)
               <<QPointF(-halfspacing, 0);*///for fine looking
        temp.addRect(whiteBound);
        return temp;
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




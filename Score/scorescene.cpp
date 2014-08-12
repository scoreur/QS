

#include <QGraphicsView>
#include <QGraphicsItem>
#include <fstream>
#include <QFile>
#include <QDebug>
#include "scoreitem.h"
#include "scorescene.h"
#include <QPainter>


/// @brief constructor of ScoreScene
ScoreScene::ScoreScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName)
{
    setItemIndexMethod(BspTreeIndex);

    if(fileName != ""){
        len = load(fileName);
    }else{
        len = 0;
        setName("Untitled.txt");
    }

    if(len == 0){
        len = 88;
        for(quint32 i=0;i<len;++i){
            note.push_back(i);
            dura.push_back(6);
        }
    }
    qreal temp_x = 10, temp_y = ScoreItem::halfnoteheight + 10;
    int totaldura = 0;
    ScoreLine *scoreLine = new ScoreLine(this, 0, 0);
    scoreLine->setPos(0,temp_y);
    int j = 0;
    scoreLine->text = QString::number(++j);

    for(quint32 i=0;i<len;++i){//typesetting

        temp_x += (ScoreItem::notespacing * dura[i]/12.0);//add x before add score
        (new ScoreItem(note[i], dura[i],scoreLine))->setPos(temp_x, 0);

        totaldura += dura[i];
        if((totaldura) % 48==0){
            temp_x += ScoreItem::notespacing * 0.75;
            (new ScoreItem(89,12,scoreLine))->setPos(temp_x, 0);//bar
        }

        if((totaldura) % 192 == 0){//4 bars per line
            temp_y += ScoreItem::linespacing;
            scoreLine = new ScoreLine(this, 0, 0);
            scoreLine->setPos(0, temp_y);
            scoreLine->text = QString::number(++j);
            temp_x = 10;
        }
    }
}
ScoreScene::~ScoreScene(){
    qDebug()<<"scoreScene removed!";
}

quint32 ScoreScene::load(QString fileName){
    std::fstream scorein;
    scorein.open(fileName.toStdString().c_str(), std::ios::in);

    if(!scorein)
        return 0;
    else{
        int tmp;
        while(!scorein.eof()){
            scorein>>tmp;
            note.push_back(uchar(tmp+48));
            scorein>>tmp;
            dura.push_back(quint8(tmp));
        }
        scorein.close();
        return note.size();
    }

}

void ScoreScene::drawBackground(QPainter *painter, QRectF *rect){
    QPen pen(Qt::red);
    painter->setPen(pen);
    painter->drawLine(0,0, 400,400);
    qDebug()<<rect;
}

/// @brief static members
quint16 ScoreScene::pagewidth = 800;
quint16 ScoreScene::pageheight = 600;
quint8 ScoreScene::margin_left = 10;
quint8 ScoreScene::margin_right = 10;
quint8 ScoreScene::margin_top = 10;
quint8 ScoreScene::margin_bottom = 10;

ScoreLine::ScoreLine(QGraphicsScene *scene, quint32 _type, QGraphicsItem *parent):
    QGraphicsRectItem(parent), type((TYPE)_type), text(""), isPressed(false)
{
    scene->addItem(this);
    setRect(0,-ScoreItem::halfnoteheight, scene->width(),ScoreItem::linespacing);
}
void ScoreLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QFont font("Times [Adobe]", 9 , QFont::Bold);
    QPen pen(QColor(0,0,220,100));
    painter->setFont(font);
    painter->setPen(pen);
    painter->drawText(5,-8, 10,10, Qt::AlignLeft, text);
    if(isPressed)
        painter->drawRect(boundingRect());
}

void ScoreLine::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        isPressed = true;
        //qDebug()<<event->scenePos();
    scene()->update(sceneBoundingRect());
}

void ScoreLine::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        isPressed = false;
    scene()->update(sceneBoundingRect());
}

void ScoreLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    setY(event->scenePos().y());
    scene()->update(sceneBoundingRect());
}

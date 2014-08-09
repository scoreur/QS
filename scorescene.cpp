

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
    ScoreItem *score;
    for(quint32 i=0;i<len;++i){//typesetting

        score = new ScoreItem(note[i], dura[i]);
        temp_x += (ScoreItem::notespacing * dura[i]/12.0);//add x before add score
        score->setPos(temp_x, temp_y);
        addItem(score);
        totaldura += dura[i];
        if((totaldura) % 48==0){
            temp_x += ScoreItem::notespacing * 0.75;
            score = new ScoreItem(89);//bar
            score->setPos(temp_x, temp_y);
            addItem(score);

        }

        if((totaldura) % 192 == 0){//4 bars per line
            temp_y += ScoreItem::linespacing;
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
    QGraphicsRectItem(parent), type((TYPE)_type)
{
    scene->addItem(this);
}

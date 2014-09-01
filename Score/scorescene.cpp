

#include <QGraphicsView>
#include <QGraphicsItem>
#include <fstream>
#include <QFile>
#include <QDebug>
#include "scoreitem.h"
#include "scorescene.h"
#include "qspreset.h"
#include <QPainter>


/// @brief constructor of ScoreScene
ScoreScene::ScoreScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName)
{
    setItemIndexMethod(BspTreeIndex);
    setBackgroundBrush(QBrush(QPixmap(QString(":/image/staff/paper3.png"), "PNG").scaled(100,100)));
    if(fileName != ""){
        len = load(fileName);
    }else{
        len = 0;
        setName("Untitled.txt");
    }

    if(len == 0){// sample score stored inside
        len = 88;
        for(quint32 i=0;i<len;++i){
            note.push_back(i);
            dura.push_back(6);
        }
    }
    qreal temp_y = ScoreItem::halfnoteheight + 10;
    quint32 totaldura = 0;
    ScoreLine *scoreLine = 0;
    BarDecorator *bar = 0;
    QVector<quint16> *scoreX = 0;
    int start_i = 0;
    quint16 lineTicks = ScoreLine::nTicksPerBeat * ScoreLine::nBeatsPerMeasure * ScoreLine::nMeasuresPerLine;
    for(quint32 i=0;i<len;++i){//typesetting
        totaldura += dura[i];
        //qDebug()<<totaldura;

        if(totaldura == lineTicks || i == len-1){//fill one line or reach the end


            scoreLine = new ScoreLine(this, 0, 0);
            bar = (BarDecorator*)scoreLine->decorator;
            scoreLine->setPos(20, temp_y);
            lines.push_back(scoreLine);
            scoreLine->text = QString::number(lines.size());

            bar->construct((quint8*)dura.begin()+start_i, i-start_i+1);
            scoreX = &bar->scoreX;
            qDebug()<<"start: "<<start_i;


            for(i = 0; i<scoreX->size(); ++i){
                (new ScoreItem(note[start_i+i], dura[start_i+i],scoreLine))->setPos((*scoreX)[i]*ScoreItem::halfnotewidth/4.0, 0);
            }
            start_i += scoreX->size();
            i = start_i - 1;
            temp_y += ScoreItem::linespacing;
            totaldura = 0;
        }

    }
}
ScoreScene::~ScoreScene(){
    qDebug()<<"scoreScene removed!";
}

quint8 ScoreLine::nTicksPerBeat = 12;		// number of ticks per beat
quint8 ScoreLine::nBeatsPerMeasure = 4;	// number of beats per measure
quint8 ScoreLine::nMeasuresPerLine = 4;	// number of measures per line

quint32 ScoreScene::load(QString fileName){
    std::fstream scorein;
    scorein.open(fileName.toStdString().c_str(), std::ios::in);

    if(!scorein)
        return 0;
    else{
        int tmp;
        while(!scorein.eof()){
            scorein>>tmp;
            note.push_back(quint8(tmp));
            scorein>>tmp;
            dura.push_back(quint8(tmp));
        }
        qDebug()<<"load successful!";
        scorein.close();
        return note.size();
    }

}
quint32 ScoreScene::store(QString fileName){
    return fileName.size();
}

void ScoreScene::drawBackground(QPainter *painter, const QRectF *rect){
    QPen pen(Qt::red);
    painter->setPen(pen);
    painter->drawLine(0,0, 400,400);
    painter->setBrush(backgroundBrush());
    painter->drawRect(*rect);
}

void ScoreScene::append(qint16 num, uchar *notes, quint8 *duras){
    for(int i = 0; i<num; ++i){
        note.push_back(notes[i]);
        dura.push_back(duras[i]);
    }
    if(num < 0){
        for(int i = 0; i < -num; ++i){
            note.pop_back();
            dura.pop_back();
        }
    }else{
    //adjust to score line

    }
}
void ScoreScene::lineUpdate(){
    for(int i = 0; i< lines.size(); ++i)
        if(lines[i]->type == LineType::NOTES)
            ((BarDecorator*)lines[i]->decorator)->updatePath();
}

qreal BarDecorator::unitX = (qreal)ScoreItem::halfnotewidth/2.0;

/// @brief constructor of BarDecorator
BarDecorator::BarDecorator(quint8 *dura, quint32 num, quint8 _ticks, Decorator *_next):
    Decorator(_next), ticks(_ticks), duraPtr(0), temp_x(0), path(0)
{
    construct(dura, num);
}
void BarDecorator::construct(quint8 *dura, quint32 num, bool reWrite){

    quint8 tick = ticks/4;//divide one beat into four (semi-quaver)
    if(reWrite){
        lowerLine1.clear();
        lowerLine2.clear();
        scoreX.clear();
        duraPtr = 0;
        temp_x = 0;
    }
    //construct lowerLine1&2 (upArc) and scoreX;

    for(int i=0; i<num; ++i){
        //currently assuming no note accross beats&bars
        if(duraPtr % ticks + dura[i]<= ticks){//not exceed one beat
            if(dura[i]/tick == 1) scoreX << 2 * temp_x;//semi-quaver;
            switch((duraPtr % ticks)/tick){
            case 0://new start
                if(dura[i] < ticks){//not full beat
                    lowerLine1 << temp_x << 4;//full first line
                }
                switch(dura[i]/tick){
                case 1://semi-quaver
                    lowerLine2 << temp_x << 1; break;
                case 2:
                    scoreX << 2 * temp_x + 1; break;
                case 3://dot needed
                    lowerLine2 << temp_x + 3 << 1;
                    scoreX << 2 * temp_x + 1;
                    dotX << 2 * (temp_x + 1); break;
                case 4:
                    scoreX << 2 * temp_x + 3; break;
                }
                break;
            case 1://
                switch(dura[i]/tick){
                case 1://semi-quaver
                    lowerLine2.back() += 1;
                    break;
                case 2:
                    scoreX << 2 * temp_x + 1; break;
                case 3:
                    scoreX << 2 * temp_x + 1;
                    dotX << 2 * (temp_x + 1); break;
                }
                break;
            case 2:
                switch(dura[i]/tick){
                case 1://semi-quaver
                    lowerLine2 << temp_x << 2; break;
                    break;
                case 2:
                    scoreX << 2 * temp_x + 1; break;
                }
                break;
            case 3: break;
            }
        }else{// TODO: finish this
            scoreX << 2 * temp_x + 1;
            temp_x += 1;
        }
        duraPtr += dura[i];
        temp_x += dura[i]/tick;
        if(duraPtr%ticks == 0){
            temp_x += 1;//reserve for interval between beat
            if((duraPtr/ticks) % 4 == 0)
                temp_x += 1;//reserve for bar
        }

    }
    updatePath();


}

void BarDecorator::paint(QPainter *painter){
    if(path != 0) painter->drawPath(*path);
    if(next != 0) next->paint(painter);
}
void BarDecorator::updatePath(){
    delete path; path = new QPainterPath;
    if(!lowerLine1.empty()){
        QVector<quint16>::iterator iter;
        qreal  temp_y = ScoreItem::notesize*0.5-2;
        for(iter = lowerLine1.begin(); iter != lowerLine1.end();iter += 2){
            path->moveTo(QPointF((*iter - 0.5) * unitX, temp_y));
            path->lineTo(QPointF((*(iter+1)+*iter-0.5) * unitX,temp_y));
        }
        if(!lowerLine2.empty()){
            temp_y += 2;
            for(iter = lowerLine2.begin(); iter != lowerLine2.end(); iter += 2){
                path->moveTo(QPointF((*iter-0.5) * unitX,temp_y));
                path->lineTo(QPointF((*(iter+1)+*iter-0.5) * unitX,temp_y));
            }
        }
    }
    if(!upArc.empty()){

        qreal archeight = 4;
        qreal  temp_y = - ScoreItem::halfnoteheight - 1;
        for(QVector<quint16>::iterator iter = upArc.begin(); iter != upArc.end(); iter += 2)
            path->arcTo(*iter * unitX, temp_y, *(iter+1) * unitX, archeight, 180 * 16, -180 *16);

    }
    for(int i=0; i< duraPtr/ticks/4; ++i){
        path->moveTo((21*i+19.5)*unitX, -ScoreItem::halfnoteheight*0.75);
        path->lineTo((21*i+19.5)*unitX, ScoreItem::halfnoteheight*0.75);
    }
}

/// @brief constructor of ScoreLine
ScoreLine::ScoreLine(QGraphicsScene *scene, quint32 _type, QGraphicsItem *parent):
    QGraphicsRectItem(parent), type((LineType)_type), text(""), decorator(0), isPressed(false)
{
    scene->addItem(this);
    setRect(-20,-ScoreItem::halfnoteheight,
            QSPreset::scorePaddingRect.width(), ScoreItem::linespacing);
    switch(type){
    case NOTES:
        decorator = new BarDecorator();
        break;
    case TITLE:
         ;
        break;
    case AUTHOR:
        ;
        break;
    case LYRIC://include the chord notation
        ;
        break;
    case FOOTTER:
        ;
        break;
    default: break;
    }
}
void ScoreLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(decorator != 0){
        decorator->paint(painter);
    }
    QFont font("Times [Adobe]", 9 , QFont::Bold);
    QPen pen(QColor(0,0,220,50));
    painter->setFont(font);
    painter->setPen(pen);
    painter->drawText(-15,-8, 10,10, Qt::AlignLeft, text);


    if(isPressed)
        painter->drawRect(boundingRect());
}


void ScoreLine::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        isPressed = true;
    mouseY = mapFromScene(event->scenePos()).y();
        //qDebug()<<event->scenePos();
    scene()->update(sceneBoundingRect());
}

void ScoreLine::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        isPressed = false;
    mouseY = 0;
    scene()->update(sceneBoundingRect());
}

void ScoreLine::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    setY(event->scenePos().y()-mouseY);
    scene()->update(sceneBoundingRect());
}

qreal ScoreLine::mouseY = 0;

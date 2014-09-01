#ifndef SCORESCENE_H
#define SCORESCENE_H

#include "qsscene.h"
#include "qspreset.h"
#include "decorator.h"
#include <QVector>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class ScoreLine;

class ScoreScene : public QSScene
{

public:
    ScoreScene(QGraphicsView *view, QString fileName = "");
    ~ScoreScene();
    //load score.txt
    quint32 load(QString fileName);
    //save score.txt
    quint32 store(QString fileName);
    //append new notes; when num<0, remove abs(num) notes from the back
    void append(qint16 num, uchar * notes = 0, quint8 * duras = 0);
    void lineUpdate();
protected:
    void drawBackground(QPainter *painter, const QRectF *rect);


private:
    QVector<uchar> note;
    QVector<quint8> dura;
    QVector<ScoreLine *> lines;
    quint32 len;
};

class ScoreLine : public QGraphicsRectItem
{
public:
    ScoreLine(QGraphicsScene *scene, quint32 _type, QGraphicsItem *parent = 0);
    ~ScoreLine(){}

    LineType type;


    QString text;//
    Decorator *decorator;
    static quint8 nTicksPerBeat, nBeatsPerMeasure, nMeasuresPerLine;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
    bool isPressed;
    static qreal mouseY;

};


class BarDecorator : public Decorator{
public:
    BarDecorator(): ticks(12), duraPtr(0), temp_x(0), path(0){}
    BarDecorator(quint8 *dura, quint32 num,
                 quint8 _ticks = ScoreLine::nTicksPerBeat, Decorator *_next = 0);
    ~BarDecorator(){delete path;}
    void paint(QPainter *painter);
    static qreal unitX;
    QVector<quint16>scoreX;		//scoreItem pos multiplied by 2
    QVector<quint16>dotX;		//attached dot pos multiplied by 2
public slots:
    void construct(quint8 *dura, quint32 num, bool reWrite = true);
    void updatePath();

private:

    quint8 ticks;			//how many ticks in one beat
    quint8 duraPtr;			//current duration pointer
    quint16 temp_x;
    QPainterPath *path;			//store the painting cmd
    QVector<quint16>lowerLine1;
    QVector<quint16>lowerLine2;
    QVector<quint16>upArc;



};

#endif // SCORESCENE_H

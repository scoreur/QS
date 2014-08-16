#ifndef SCORESCENE_H
#define SCORESCENE_H

#include "../qsscene.h"
#include <QVector>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class ScoreLine;

class ScoreScene : public QSScene
{

public:
    ScoreScene(QGraphicsView *view, QString fileName = "");
    ~ScoreScene();
    quint32 load(QString fileName);
    quint32 store(QString fileName);

    static quint16 pagewidth;
    static quint16 pageheight;
    static quint8 margin_left;
    static quint8 margin_right;
    static quint8 margin_top;
    static quint8 margin_bottom;

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

    static quint8 padding_left;
    static quint8 padding_right;
    static quint8 padding_top;
    static quint8 padding_bottom;

    enum TYPE{
        SCORE = 0,
        TITLE,
        AUTHOR,
        COMMENT, //include the chord notation;
        LYRIC,
        FOOTTER,
    };
    TYPE type;


    QString text;//

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
    bool isPressed;
};


#endif // SCORESCENE_H

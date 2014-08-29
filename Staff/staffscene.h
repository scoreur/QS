#ifndef STAFFSCENE_H
#define STAFFSCENE_H

#include "../qsscene.h"
#include "../qspreset.h"
#include "staffitem.h"
#include <QGraphicsRectItem>
#include <QPainter>
#include <QVector>

// forward declarations
class StaffImages;
class StaffLine;



class StaffScene : public QSScene{
public:
    StaffScene(QGraphicsView *view, QString fileName);
    ~StaffScene();
    quint32 load(QString fileName);
    quint32 store(QString fileName);
    QVector<StaffLine*> lines;
private:


};

class StaffMeasure : QGraphicsRectItem{

public:
    StaffMeasure(QGraphicsItem *parent);
    ~StaffMeasure(){}

private:

};



class StaffLine : public QGraphicsRectItem{
public:
    StaffLine(QGraphicsScene *scene, quint32 _type, QGraphicsItem *parent = 0);
    ~StaffLine(){}

    LineType type;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);


};

#endif // STAFFSCENE_H

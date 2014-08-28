#ifndef STAFFITEM_H
#define STAFFITEM_H

#include <QGraphicsObject>
#include <QPainter>



class StaffItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit StaffItem(QGraphicsItem *parent = 0);
    ~StaffItem(){}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);


signals:

public slots:
    void showMark(bool f = true);// show sharp/flat/nature sign

private:
    quint8 pitch;
    quint8 dura;
    qint8 mark;
    bool markShowed;

};

class KeySignature : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit KeySignature(QGraphicsItem *parent = 0, qint8 _clef = 0, qint8 _id = 0);
    ~KeySignature(){}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    QRectF boundingRect() const{
        return bound;
    }


    qint8 clef;//0 for treble_clef, 1 for bass_clef (extend: 2 for alto_clef)
     qint8 id;// from -6 to 5 for 12 keys
    // TODO: add beat signature
    QVector<QPointF> places;
    void constructPlaces();
private:
    QRectF bound;
    QPixmap clefImg;
    QPixmap markImg;
    QPointF clefPos;
};

#endif // STAFFITEM_H

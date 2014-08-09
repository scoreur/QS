#ifndef SCOREITEM_H
#define SCOREITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>


class ScoreItem: public QGraphicsItem
{
public:
    ScoreItem(uchar _pitch, uchar _duration = 12);
    ~ScoreItem(){}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath shape() const;
    QRectF boundingRect() const{
        return bound;
    }

    static qint8 keyPitch;//signed!
    static quint8 halfnotewidth;
    static quint8 halfnoteheight;
    static quint8 notespacing;
    static quint8 linespacing;
    static quint8 notesize;
    static QColor presetColor;
    static QBrush selectedBrush;
public slots:
    void noteUpdate(qint8 adjust=0);
    static void scorePreset(int mode, int arg);
protected slots:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    uchar pitch;//0~87 for note, 88 as rest, 89 as bar
    uchar duration;//multiplied by 12
    qint8 note;//0~7
    qint8 semi;//b, #
    qint8 oct;//number of dots above(+) / below(-)
    QRectF bound;
    QColor color;
    int mouseY;
};




#endif // SCOREITEM_H

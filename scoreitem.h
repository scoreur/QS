#ifndef SCOREITEM_H
#define SCOREITEM_H
#include <QGraphicsItem>
#include <QPainter>


class ScoreItem: public QGraphicsItem
{
public:
    ScoreItem(uchar _pitch, uchar _duration=12);
    ~ScoreItem(){}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath shape() const;
    QRectF boundingRect() const;

    static qint8 keyPitch;
    static qint8 halfnotewidth;
    static qint8 halfnoteheight;
    static qint8 notespacing;
    static qint8 linespacing;
    static qint8 notesize;

public slots:
    void noteUpdate(qint8 adjust=0);

private:
    uchar pitch;//0~87 for note, 88 as rest, 89 as bar
    uchar duration;//multiplied by 12
    qint8 note;//0~7
    qint8 semi;//b, #
    qint8 oct;//number of dots above(+) / below(-)
    QRectF bound;
};

#endif // SCOREITEM_H

#ifndef PIANOKEY_H
#define PIANOKEY_H
#include <QGraphicsItem>
#include <QPainter>

class PianoKey: public QGraphicsItem
{
public:
    PianoKey(uchar);
    ~PianoKey(){};

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    QPainterPath shape() const;

    QColor color();

    static qreal halfspacing;   //half interval of white keys
    static qreal halfwidth;     //half width fo black keys
    static qreal whiteheight;   //
    static qreal blackheight;   //
    static QPainterPath defaultKeyShape(uchar);//
    static bool isWhite(uchar);
    enum TYPE{
        BLACK = 0,
        WHITE_LEFT,
        WHITE_NORMAL,
        WHITE_RIGHT,
        WHITE_WHOLE
    };

private:
    const uchar id;             //key id
    const QPainterPath path;    //key shape
    const QRectF bound;
    bool isPressed;

};




#endif // PIANOKEY_H

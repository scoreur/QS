#ifndef PIANOKEY_H
#define PIANOKEY_H
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
//#include <QString>


class PianoKey: public QGraphicsItem
{
public:
    PianoKey(uchar, QGraphicsItem *parent = 0);
    ~PianoKey(){
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const{
        return bound;
    }
    QPainterPath shape() const{
        return path;
    }

    QColor color();

    static QString soundpath;

    static qreal halfspacing;   //half interval of white keys
    static qreal halfwidth;     //half width fo black keys
    static qreal whiteheight;   //
    static qreal blackheight;   //
    static QPainterPath defaultKeyShape(uchar);//
    static QRectF defaultKeyBound(uchar);
    static bool isWhite(uchar);
    bool isWhite() const{
        return is_white;
    }
    enum TYPE{
        BLACK = 0,
        WHITE_LEFT,
        WHITE_NORMAL,
        WHITE_RIGHT,
        WHITE_WHOLE
    };
protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
protected:
    void press(bool f = true);
private:
    const uchar id;             //key id
    const bool is_white;        //key color
    const QPainterPath path;    //key shape
    const QRectF bound;
    bool is_pressed;



};




#endif // PIANOKEY_H

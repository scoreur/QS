#ifndef PIANOKEY_H
#define PIANOKEY_H
#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QKeyEvent>
//#include <QString>
//#include <QThread>
#include <QSound>
#define HAVE_NATURAL_KEYSOUND //disable this when have no keysound

class PianoKey: public QGraphicsObject
{
//no need to
public:
    PianoKey(uchar, QGraphicsItem *parent = 0);
    ~PianoKey(){
        delete keysound;

    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const{
        return bound;
    }
    QPainterPath shape() const{
        return path;
    }

    QColor color();

    friend class KeyScene;
    static QString soundpath;

    static qreal halfspacing;   //half interval of white keys
    static qreal halfwidth;     //half width fo black keys
    static qreal whiteheight;   //
    static qreal blackheight;   //
    static QRectF blackBound;
    static QRectF whiteBound;
    static QPainterPath defaultKeyShape(uchar);//
    static quint16 latency;
    static quint16 keyTimeAccuracy;
    static bool isWhite(uchar);
    static quint8 scope;
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
public slots:
    void keyPress();
    void keyRelease();


protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    //void keyPressEvent(QKeyEvent *event);
    //void keyReleaseEvent(QKeyEvent *event);
    void press(bool f = true);
private:
    const uchar id;             //key id
    const bool is_white;        //key color
    const QPainterPath path;    //key shape
    const QRectF bound;
    bool is_pressed;

    QSound *keysound;
    //QThread *keythread;




};




#endif // PIANOKEY_H

#ifndef QSSCENE_H
#define QSSCENE_H

#include <QGraphicsScene>
#include <QString>
#include <QAction>
#include <QTime>

class QGraphicsItem;
class QGraphicsView;


class KeyScene : public QGraphicsScene
{
public:
    KeyScene(QGraphicsView *view, QWidget *parent);
    ~KeyScene();
    QPointF spectrum[88];
    friend class PianoKey;
signals:
    void keyInput(quint8);
protected:
    void drawForeground(QPainter *painter, const QRectF &rect);
private:
    QGraphicsItem *board;
    QTime keyTime;
};

class QSScene : public QGraphicsScene{

public:
    QSScene(QGraphicsView *view, QString fileName = "");
    ~QSScene();
    QString Name() const{
        return name;
    }
    QAction *Opened() const{
        return act;
    }
    void setName(QString fileName);
    virtual quint32 load(QString fileName) = 0;
    virtual quint32 store(QString fileName) = 0;





private:
    QString name;
    QAction *act;
};






#endif // QSSCENE_H

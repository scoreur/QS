#ifndef QSWINDOW_H
#define QSWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "pianokey.h"



namespace Ui {
class QSWindow;
}

class QSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QSWindow(QWidget *parent = 0);
    ~QSWindow();
public slots:
    void openFile();
    void saveFile();
    void saveFileAs();

private:
    Ui::QSWindow *ui;
    QGraphicsScene *wavScene, *keyScene, *scoreScene;
    QString openFileName, saveFileName, tempFileName;
    static qreal wavViewSec;//seconds in view

    void keyBoardInit();
    void wavFormInit();
    void scoreInit();

    qulonglong wavLoad();
public:
    short *wavldata, *wavrdata;//left & right channel pcm data
    qulonglong wavdatasize;
    //score structure undefined!
private slots:
    void on_verticalScrollBar_valueChanged(int value);

public slots:
    void wavFormUpdate();
    void scoreUpdate();
};


class WavFormChannel: public QGraphicsItem{
public:
    WavFormChannel(qulonglong _datasize, short *_data, qreal _intv, qreal _amp);
    ~WavFormChannel(){
        delete data;
    };

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    QPainterPath shape() const;

private:

    qulonglong datasize;
    qreal interval;
    qreal amplitude;
    QPointF *data;
    const QRectF bound;

};

#endif // QSWINDOW_H

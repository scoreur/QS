#ifndef QSVIEW_H
#define QSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QLabel>

class QSView : public QGraphicsView
{
public:
    QSView(QWidget *parent, int w, int h);
    ~QSView(){}

protected:
    virtual void wheelEvent(QWheelEvent *event);
};


class WavView : public QSView{

public:
    WavView(QWidget *parent, int w, int h);
    ~WavView(){}

    QLabel *mouseTime;
signals:
    void KeyInput(quint8);
protected:
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
};

class ScoreView : public QSView{

public:
    ScoreView(QWidget *parent, int w, int h);
    ~ScoreView(){}
protected:
    void wheelEvent(QWheelEvent *event);
};

class StaffView : public QSView{

public:
    StaffView(QWidget *parent, int w, int h);
    ~StaffView(){}
protected:
    void wheelEvent(QWheelEvent *event);
};
#endif // QSVIEW_H

#include "qsview.h"
#include <QDebug>
#include <QScrollBar>
#include <QGraphicsScene>
#include "Wave/wavscene.h"
#include <QPointF>
#include <QTime>

QSView::QSView(QWidget *parent, int w, int h):
    QGraphicsView(parent)
{
    resize(w,h);
    setEnabled(true);
    show();
    setDragMode(NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);


}

void QSView::wheelEvent(QWheelEvent *event){
    Q_UNUSED(event)

}

WavView::WavView(QWidget *parent, int w, int h):
    QSView(parent, w, h){
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(QGraphicsView::CacheNone);

}

void WavView::wheelEvent(QWheelEvent *event){
    QWheelEvent *tmp = new QWheelEvent(event->pos(), event->angleDelta().x(), event->buttons(), 0, Qt::Horizontal);
    QGraphicsView::wheelEvent(tmp);
    delete tmp;
    qreal sc = 1+event->angleDelta().y()/200.0;
    if(event->angleDelta().x()<-100 || event->angleDelta().x()>100)
        return;
    if(sc>1 && matrix().m11()>4) return;
    if(sc<1 && matrix().m11()<0.25) return;
    if(sc>1.08) sc = 1.08;
    if(sc<0.92) sc = 0.92;
    scale(sc, 1);
}
void WavView::mouseDoubleClickEvent(QMouseEvent *event){
    Q_UNUSED(event);
    setTransform(QTransform());
    verticalScrollBar()->setValue(0);
}

/// @brief ScoreView constructor
ScoreView::ScoreView(QWidget *parent, int w, int h):
    QSView(parent, w, h){
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

}
void ScoreView::wheelEvent(QWheelEvent *event){
    Q_UNUSED(event)
    verticalScrollBar()->show();
}

StaffView::StaffView(QWidget *parent, int w, int h):
    QSView(parent, w, h){
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

}
void StaffView::wheelEvent(QWheelEvent *event){
    Q_UNUSED(event)
    verticalScrollBar()->show();
}

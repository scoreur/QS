#include "qsview.h"
#include <QDebug>
#include <QScrollBar>

QSView::QSView(QWidget *parent, int w, int h):
    QGraphicsView(parent)
{
    resize(w,h);
    setEnabled(true);
    show();
    setDragMode(RubberBandDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

void QSView::wheelEvent(QWheelEvent *event){

}

WavView::WavView(QWidget *parent, int w, int h):
    QSView(parent, w, h){
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

}
void WavView::wheelEvent(QWheelEvent *event){
    horizontalScrollBar()->show();
}

/// @brief ScoreView constructor
ScoreView::ScoreView(QWidget *parent, int w, int h):
    QSView(parent, w, h){
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

}
void ScoreView::wheelEvent(QWheelEvent *event){
    verticalScrollBar()->show();
}

StaffView::StaffView(QWidget *parent, int w, int h):
    QSView(parent, w, h){
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

}
void StaffView::wheelEvent(QWheelEvent *event){
    verticalScrollBar()->show();
}

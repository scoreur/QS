#include "staffscene.h"
#include <QDebug>

/// @brief constructor of StaffScene
StaffScene::StaffScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName)
{
    if(fileName.isEmpty())
        setName("Untitled.mid");
    setBackgroundBrush(QBrush(QPixmap(QString(":/image/staff/paper2.png"), "PNG").scaled(100,100)));

    quint8 test_notes[8] = {39,41,43,44,46,48,50,51};


}
StaffScene::~StaffScene(){
    qDebug()<<"staffScene removed!";
}
quint32 StaffScene::load(QString fileName){
    return fileName.size();
}
quint32 StaffScene::store(QString fileName){
    return fileName.size();
}

StaffMeasure::StaffMeasure(QGraphicsScene *scene, QGraphicsItem *parent):
    QGraphicsRectItem(parent)
{
    scene->addItem(this);
}


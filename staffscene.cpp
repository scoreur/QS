#include "staffscene.h"
#include <QDebug>

/// @brief constructor of StaffScene
StaffScene::StaffScene(QGraphicsView *view, QString fileName)
    : QSScene(view, fileName)
{
    if(fileName.isEmpty())
        setName("Untitled.mid");

}
StaffScene::~StaffScene(){
    qDebug()<<"staffScene removed!";
}


#ifndef STAFFSCENE_H
#define STAFFSCENE_H

#include "../qsscene.h"

class StaffScene : public QSScene{
public:
    StaffScene(QGraphicsView *view, QString fileName);
    ~StaffScene();
    quint32 load(QString fileName){return 0;}
    quint32 store(QString fileName){return 0;}
private:

};


#endif // STAFFSCENE_H

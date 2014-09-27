#include "General/qswindow.h"
#include <QApplication>
#include <QLabel>
#include <QTimer>

void welcomeEffect();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSWindow w;
    int effectDelay = 0;
    //welcomeEffect();
    //effectDelay = 3000;
    QTimer::singleShot(effectDelay, &w, SLOT(show()));

    return a.exec();
}

/**
 * @brief show welcome effect, enabled when released
 */
void welcomeEffect(){
    QLabel *welc = new QLabel(0,Qt::FramelessWindowHint);
    QPixmap pixmap(QString(":/image/lm.jpg"),"JPG");
    welc->resize(200,200);
    welc->setPixmap(pixmap.scaled(200,200));
    welc->show();
    QTimer::singleShot(3000, welc, SLOT(close()));
    QTimer::singleShot(3500, welc, SLOT(deleteLater()));
}

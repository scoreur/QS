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

// about info
QString QSAbout::version = "0.9";
QString QSAbout::date = "Oct. 1, 2014";
QString QSAbout::copyright = "© QtScoreur";
QString QSAbout::domainName = "scoreur.net";
QString QSAbout::appName = "QtScoreur";


QString getAbout(int attr){
    switch(attr){
    case 0:
        return QSAbout::version;
    case 1:
        return QSAbout::date;
    case 2:
        return QSAbout::domainName;
    case 3:
        return QSAbout::appName;
    case 4:
        return QSAbout::copyright;
    }
    return QString();
}



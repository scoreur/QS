#include "qswindow.h"
#include <QApplication>
#include <QSound>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSWindow w;
    w.show();

    return a.exec();
}

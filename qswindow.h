#ifndef QSWINDOW_H
#define QSWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QVector>
#include "qsscene.h"
#include "qspreset.h"
#include <QThread>
//#include <QSoundEffect>

//class musicPlay;

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
    void closeFile();

private:
    Ui::QSWindow *ui;
    QGraphicsScene *keyScene;
    QSPreset *preset;
    QString openFileName, saveFileName, tempFileName;
    //musicPlay *musicthread;


private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void addScene(QGraphicsView *view, QString fileName = "");
    void switchScene(QAction*);

    void changePreset();

};



#endif // QSWINDOW_H

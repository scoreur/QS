#ifndef QSWINDOW_H
#define QSWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <vector>
#include "qsscene.h"
#include <QThread>
#include <QSoundEffect>

class musicPlay;

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

private:
    Ui::QSWindow *ui;
    QGraphicsScene *keyScene;
    std::vector<QGraphicsScene *> wavScene, scoreScene;
    int currentWavId, currentScoreId;
    QString openFileName, saveFileName, tempFileName;
    musicPlay *musicthread;


public:

    qulonglong wavdatasize;
    //score structure undefined!
private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void switchWavScene(QAction*);

};




#endif // QSWINDOW_H

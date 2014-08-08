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
    void closeFile();

private:
    Ui::QSWindow *ui;
    QGraphicsScene *keyScene;
    QVector<QGraphicsScene *> wavScene, scoreScene;
    int currentWavId, currentScoreId;
    QVector<QAction *> wavOpened, scoreOpened;
    QString openFileName, saveFileName, tempFileName;
    musicPlay *musicthread;
    QSPreset *preset;

private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void addWav(QString fileName = "");
    void addScore(QString fileName = "");
    void switchWavScene(QAction*);
    void switchScoreScene(QAction *);
    void changePreset();

};




#endif // QSWINDOW_H

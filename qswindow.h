#ifndef QSWINDOW_H
#define QSWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QVector>
#include "qsscene.h"
#include "qsview.h"
#include "qspreset.h"
#include <QThread>
#include <QEvent>

#include <QMediaPlayer>
#include <QSlider>
#include <QAbstractButton>


namespace Ui {
class QSWindow;
}

class QSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QSWindow(QWidget *parent = 0);
    ~QSWindow();
    
    static QSize defaultWinSize;
    static QRect keyViewRect;
    static QRect wavViewRect;
    static QRect scoreViewRect;
    static QRect staffViewRect;
    
public slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void closeFile();

    //music play
    void musicPlay();
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void mediaStateChanged(QMediaPlayer::State state);
    void setPosition(int position);

protected slots:
    void closeEvent(QCloseEvent *);


private:
    Ui::QSWindow *ui;
    QSView *keyView, *wavView, *scoreView, *staffView;
    QSPreset *preset;
    QString openFileName, saveFileName, tempFileName;

    QMediaPlayer *mediaPlayer;
    QAbstractButton *playButton;
    QSlider *positionSlider;


private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void addScene(QGraphicsView *view, QString fileName = "");
    void switchScene(QAction*);

    void changePreset();

};



#endif // QSWINDOW_H

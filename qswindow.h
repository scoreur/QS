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
#include <QTextEdit>

#include "qsplayer.h"


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
    static QSize defaultTabSize;
    static QRect keyViewRect;
    static QRect wavViewRect;
    static QRect scoreViewRect;
    static QRect staffViewRect;

    
public slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void closeFile();

    void generalPreset(int mode, int arg);
    void generalPreset(int mode, QSize size);
    void keyInput(quint8);


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
    QSView *wavView, *scoreView, *staffView,  *keyView;//keyview must be created after wavView
    KeyScene *keyScene;
    QSPreset *preset;
    QString openFileName, saveFileName, tempFileName;

    QMediaPlayer *mediaPlayer;
    QAbstractButton *playButton;
    QSlider *positionSlider;
    QSPlayer *musicthread;
    void preloadConnect();


private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void addScene(QGraphicsView *view, QString fileName = "");
    void switchScene(QAction*);

    void changePreset();

};



#endif // QSWINDOW_H

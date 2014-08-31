#ifndef QSWINDOW_H
#define QSWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QVector>
#include "qsscene.h"
#include "qsview.h"
#include "html5applicationviewer.h"
#include "qspreset.h"
#include <QThread>
#include <QEvent>
#include <QTextEdit>

#include "qsplayer.h"

const QString domainName = "www.scoreur.cn";
const QString appName = "QtScoreur";


namespace Ui {
class QSWindow;
}

class QSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QSWindow(QWidget *parent = 0);
    ~QSWindow();
    
    friend class QSPreset;

public slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void closeFile();

    void generalPreset(int mode, int arg);
    void generalPreset(int mode, QSize size);
    void keyInput(quint8, quint8 dura);


    //music play
    void musicPlay();
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void mediaStateChanged(QMediaPlayer::State state);
    void setPosition();

protected slots:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);


private:
    Ui::QSWindow *ui;
    QSView *wavView, *scoreView, *staffView,  *keyView;//keyview must be created after wavView
    Html5ApplicationViewer *webView;
    KeyScene *keyScene;

    QString openFileName, saveFileName, tempFileName;

    QMediaPlayer *mediaPlayer;
    QAbstractButton *playButton;
    QSlider *positionSlider;
    QSPlayer *musicthread;
    QSPreset *preset;
    void preloadConnect();


private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void addScene(QGraphicsView *view, QString fileName = "");
    void switchScene(QAction*);
    void displayKeyBoard();
    void displaySpectrum();

    void changePreset();

};



#endif // QSWINDOW_H

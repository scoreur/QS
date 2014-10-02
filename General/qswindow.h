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
#include <QLabel>
#include <QPaintEvent>
#include "qsrecorder.h"
#include "qsplayer.h"

class QSAbout{
public:
    QSAbout(){}

    static QString version, date;
    static QString domainName, appName;
    static QString copyright;

};
QString getAbout(int attr);


class WavFile;

namespace Ui {
class QSWindow;
}

/**
 * @brief The QSWindow class, main window of the Qt project
 */
class QSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QSWindow(QWidget *parent = 0);
    ~QSWindow();
    
    friend class QSPreset;
    friend class WavFile;
signals:
    Q_INVOKABLE void addFromLameSignal(const QString &fileName);
public slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void closeFile();

    void generalPreset(int mode, int arg);
    void generalPreset(int mode, QSize size);
    /**
     * @brief keyInput, read the keyboard input into score scene
     * @param dura
     */
    void keyInput(quint8, quint8 dura);


    /**
     * @brief musicInit, initiate the music thread
     */
    void musicInit();
    /**
     * @brief musicPlay, play the audio file
     */
    void musicPlay();
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void mediaStateChanged(QMediaPlayer::State state);
    void setPosition();
    void addFromLame(const QString &fileName){
        addScene(wavView, fileName);
    }

    /**
     * @brief onsetDetect
     * @param wavFile
     * @param fileName
     * @return
     */
    quint32 onsetDetect(const WavFile &wavFile, const QString &fileName);
    quint32 onsetDetect(const WavFile &wavFile, QVector<quint8>&pitch, QVector<qreal>&onset);



protected slots:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);


protected:
    void paintEvent(QPaintEvent *event);


private:
    Ui::QSWindow *ui;
    QSView *wavView, *scoreView, *staffView,  *keyView;//keyview must be created after wavView
    Html5ApplicationViewer *webView;
    KeyScene *keyScene;

    QString openFileName, saveFileName, tempFileName;

    QMediaPlayer *mediaPlayer;
    QAbstractButton *playButton;
    QLabel *playTime;
    QSlider *positionSlider;
    QSPlayer *musicthread;
    QSRecorder *recorder;
    QSPreset *preset;
    void preload();


private slots:
    void on_verticalScrollBar_valueChanged(int value);
    void addScene();
    void addScene(QGraphicsView *view, const QString fileName = "");
    void addScene(QGraphicsView *view, const WavFile &_wavFile, const QString &fileName);
    void switchScene(QAction*);
    void displayKeyBoard();
    void displaySpectrum();
    void scoreToWav();
    void wavToScore();
    void record();


    /**
     * @brief changePreset, change the preset parameters
     */
    void changePreset();

};



#endif // QSWINDOW_H

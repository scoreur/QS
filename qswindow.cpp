#include "qswindow.h"
#include "ui_qswindow.h"
#include "pianokey.h"
#include "scoreitem.h"
#include "wavchannel.h"
#include "qsscene.h"
#include <QFileDialog>
#include <QFile>
#include <QIcon>
#include <QDebug>
#include <fstream>
#include <QSoundEffect>

class musicPlay : public QThread{

public:
    musicPlay(QUrl _url): stopped(false),
    url(_url){
        effect = new QSoundEffect(this);
        effect->setSource(url);
        effect->setLoopCount(1);
        effect->play();
        effect->setVolume(0.5f);
    }
    ~musicPlay(){

    }

    void stop(){
        stopped = true;
    }

protected:
    void run();

private:
    volatile bool stopped;
    QUrl url;
    QSoundEffect *effect;


};
void musicPlay::run(){
    if(stopped){
        effect->stop();
    }
}


/// @brief constructor of QS mainwindow
QSWindow::QSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QSWindow),
    currentWavId(0),
    currentScoreId(0),
    openFileName(""),
    saveFileName("")
{

    ui->setupUi(this);

    //set icon
    QIcon iconQS(":/image/QScore.jpg");
    setWindowIcon(iconQS);

    //initialize
    keyScene = new KeyScene(this, ui->keyView);
    wavScene.push_back((new WavScene(this, ui->wavView)));
    ui->menuOpened->addAction("wav0: Untitled.wav");
    scoreScene.push_back(new ScoreScene(this, ui->scoreView));

    //enhancing effect
    ui->statusBar->showMessage("Welcome to QtScoreur!");


    //connettion:
    connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));
    ui->actionOpen_File->setShortcut(QKeySequence::Open);
    connect(ui->actionSave_File, SIGNAL(triggered()), this, SLOT(saveFile()));
    ui->actionSave_File->setShortcut(QKeySequence::Save);
    connect(ui->actionSave_File_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    ui->actionSave_File_as->setShortcut(QKeySequence::SaveAs);
    connect(ui->menuOpened, SIGNAL(triggered(QAction*)), this, SLOT(switchWavScene(QAction*)));


}

QSWindow::~QSWindow()
{
    if(musicthread->isRunning()){
        musicthread->stop();
        qDebug()<<"music stop!";
    }
    delete ui;


}


//file control module
void QSWindow::openFile(){
    if(ui->tabWidget->currentIndex()==0)
        openFileName = QFileDialog::getOpenFileName(this, "Open .wav File: ", QDir::currentPath(), "*.wav");
    else
        openFileName = QFileDialog::getOpenFileName(this, "Open .txt File: ", QDir::currentPath(), "*.txt");

    if(openFileName.isEmpty())
        ui->statusBar->showMessage("open file unchosen!", 5000);
    else{
        ui->statusBar->showMessage(QString("open: %1; save: %2").arg(openFileName).arg(saveFileName));
        if(ui->tabWidget->currentIndex()==0){

            ui->menuOpened->addAction(QString("wav%1: ~%2").arg(wavScene.size())
                                      .arg(openFileName.right(10)));
            wavScene.push_back(new WavScene(this, ui->wavView, openFileName));
            musicthread = new musicPlay(QUrl(QString("file://") + openFileName));
            musicthread->start();

        }
        else
        {
            scoreScene.push_back((new ScoreScene(this, ui->scoreView, openFileName)));
            //qDebug()<<"score update";
        }

    }

}
void QSWindow::saveFile(){
    if(ui->tabWidget->currentIndex()==1)
        saveFileName = QFileDialog::getSaveFileName(this, "Save .wav File: ", QDir::currentPath(), "*.wav");
    else
        saveFileName = QFileDialog::getSaveFileName(this, "Save .txt File: ", QDir::currentPath(), "*.txt");

    if(saveFileName.isEmpty())
        ui->statusBar->showMessage("save file unchosen!", 5000);
    else
    {
        if(QDir(saveFileName).exists()){
            ui->statusBar->showMessage(QString("the file %1 already exists! Please select another!").arg(saveFileName),5000);
            saveFileName = "";
        }
        else{
            ui->statusBar->showMessage(QString("open: %1; save: %2").arg(openFileName).arg(saveFileName));
        }

    }
}
void QSWindow::saveFileAs(){

}
// ***Slots:

void QSWindow::on_verticalScrollBar_valueChanged(int value)
{
    qDebug()<<value;
}

void QSWindow::switchWavScene(QAction * act){
    QString actName = act->text();
    if(actName.left(3) == "wav"){
        ui->wavView->setScene(wavScene[actName.mid(3,1).toInt()]);
        qDebug()<<"set scene"<<actName.mid(3,1).toInt();
    }else{ // actName.left(5) == "score"
        ui->scoreView->setScene((scoreScene[actName.mid(6,1).toInt()]));
    }
}

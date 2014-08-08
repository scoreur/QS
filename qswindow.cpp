#include "qswindow.h"
#include "ui_qswindow.h"
#include "pianokey.h"
#include "scoreitem.h"
#include "wavchannel.h"
#include "qspreset.h"
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
    currentWavId(-1),
    currentScoreId(-1),
    openFileName(""),
    saveFileName("")
{

    ui->setupUi(this);
    connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));
    ui->actionOpen_File->setShortcut(QKeySequence::Open);
    connect(ui->actionSave_File, SIGNAL(triggered()), this, SLOT(saveFile()));
    ui->actionSave_File->setShortcut(QKeySequence::Save);
    connect(ui->actionSave_File_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    ui->actionSave_File_as->setShortcut(QKeySequence::SaveAs);
    connect(ui->actionClose, SIGNAL(triggered()),this, SLOT(closeFile()));
    ui->actionClose->setShortcut(QKeySequence::Close);
    connect(ui->actionPreset, SIGNAL(triggered()), this, SLOT(changePreset()));


    connect(ui->menuOpened, SIGNAL(triggered(QAction*)), this, SLOT(switchWavScene(QAction*)));
    connect(ui->menuOpened, SIGNAL(triggered(QAction*)), this, SLOT(switchScoreScene(QAction*)));
    //set icon
    QIcon iconQS(":/image/QScore.jpg");
    setWindowIcon(iconQS);

    //initialize
    keyScene = new KeyScene(this, ui->keyView);
    addWav();
    addScore();
    preset = new QSPreset(this);
    //preset->setModal(true);
    //enhancing effect
    ui->statusBar->showMessage("Welcome to QtScoreur!");

}

QSWindow::~QSWindow()
{ 
    delete preset;
    delete ui;
}


//file control module
void QSWindow::openFile(){
    openFileName = QFileDialog::getOpenFileName
            (this, "Open File: ", QDir::homePath(), "*.wav *.txt");

    if(openFileName.isEmpty())
        ui->statusBar->showMessage("open file unchosen!", 5000);
    else{
        ui->statusBar->showMessage(QString("open: %1").arg(openFileName));
        if(openFileName.endsWith("wav", Qt::CaseInsensitive)){
            addWav(openFileName);
            //musicthread = new musicPlay(QUrl(QString("file://") + openFileName));
            //musicthread->start();
        }
        else if(openFileName.endsWith("txt", Qt::CaseInsensitive))
        {
            addScore(openFileName);

        }
        else{
            ui->statusBar->showMessage("can't process such a file!");
            openFileName = "";
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
            ui->statusBar->showMessage(QString("the file %1 already exists! Please change to another!").arg(saveFileName),5000);
            saveFileName = "";
        }
        else{
            ui->statusBar->showMessage(QString("open: %1; save: %2").arg(openFileName).arg(saveFileName));
        }

    }
}
void QSWindow::saveFileAs(){

}

void QSWindow::closeFile(){
    if(ui->tabWidget->currentIndex() == 0){
        if(wavOpened.size()>1){
            delete wavScene[currentWavId];
            wavScene.remove(currentWavId);
            ui->menuOpened->removeAction(wavOpened[currentWavId]);
            delete wavOpened[currentWavId];
            wavOpened.remove (currentWavId);
            currentWavId = -1;
            emit wavOpened[wavOpened.size()-1]->triggered();
        }else{
            ui->statusBar->showMessage("only one wave file left!");
        }

    }else{
        if(scoreOpened.size()>1){
            delete scoreScene[currentScoreId];
            //qDebug()<<"step1"<<scoreScene.size();
            scoreScene.remove(currentScoreId);
            ui->menuOpened->removeAction(scoreOpened[currentScoreId]);
            delete scoreOpened[currentScoreId];
            //qDebug()<<"step3";
            scoreOpened.remove (currentScoreId);
            currentScoreId = -1;//this is important!
            emit scoreOpened[0]->triggered();
            //qDebug()<<"step5:"<<currentScoreId;
        }else{
            ui->statusBar->showMessage("only one score file left!");
        }

    }
}

// ***other Slots:

void QSWindow::on_verticalScrollBar_valueChanged(int value)
{
    qDebug()<<value;
}

void QSWindow::addWav(QString fileName){
    wavScene.push_back((new WavScene(this, ui->wavView, fileName)));
    if(fileName == ""){
        wavOpened.push_back(ui->menuOpened->addAction("~Untitled.wav"));

    }else{
        wavOpened.push_back(ui->menuOpened->addAction(QString("~%1")
                                  .arg(fileName.right(16))));

    }
    wavOpened[wavOpened.size()-1]->setCheckable(true);
    emit wavOpened[wavOpened.size()-1]->triggered();

}

void QSWindow::addScore(QString fileName){
    scoreScene.push_back((new ScoreScene(this, ui->scoreView, fileName)));
    if(fileName == ""){
        scoreOpened.push_back(ui->menuOpened->addAction("~Untitled.txt"));

    }else{
        scoreOpened.push_back(ui->menuOpened->addAction(QString("~%1")
                                  .arg(fileName.right(16))));
    }
    scoreOpened[scoreOpened.size()-1]->setCheckable(true);
    emit scoreOpened[scoreOpened.size()-1]->triggered();

}

void QSWindow::switchWavScene(QAction * act){
    int id = wavOpened.indexOf(act);
    if(id == -1) return; //not wave file
    if(id != currentWavId){ //changed
        if(currentWavId != -1)
            wavOpened[currentWavId]->setChecked(false);
        currentWavId = id;
        ui->wavView->setScene(wavScene[currentWavId]);
        qDebug()<<"set wave scene "<<currentWavId;
    }
    wavOpened[currentWavId]->setChecked(true);
    if(ui->tabWidget->currentIndex() == 1)
        ui->tabWidget->setCurrentIndex(0);
}

void QSWindow::switchScoreScene(QAction *act)
{
    int id = scoreOpened.indexOf(act);
    if(id == -1) return;//not score file
    if(id != currentScoreId){//changed
        if(currentScoreId != -1)//no score file open
            scoreOpened[currentScoreId]->setChecked(false);
        currentScoreId = id;
        ui->scoreView->setScene(scoreScene[currentScoreId]);
        qDebug()<<"set score scene "<<currentScoreId;
    }
    scoreOpened[currentScoreId]->setChecked(true);
    if(ui->tabWidget->currentIndex() == 0)
        ui->tabWidget->setCurrentIndex(1);
}

void QSWindow::changePreset(){

    preset->show();

}

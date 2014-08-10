#include "qswindow.h"
#include "ui_qswindow.h"
#include "qsview.h"

#include "wavscene.h"
#include "scorescene.h"
#include "staffscene.h"
#include "qspreset.h"

#include <QFileDialog>
#include <QFile>
#include <QIcon>
#include <QPushButton>
#include <QDebug>

/// @brief constructor of QS mainwindow
QSWindow::QSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QSWindow),
    keyView(0),
    wavView(0),
    scoreView(0),
    staffView(0),
    openFileName(""),
    saveFileName(""),
    tempFileName("")
{

    ui->setupUi(this);
    //file control buttons and shortcuts
    connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));
    ui->actionOpen_File->setShortcut(QKeySequence::Open);
    connect(ui->actionSave_File, SIGNAL(triggered()), this, SLOT(saveFile()));
    ui->actionSave_File->setShortcut(QKeySequence::Save);
    connect(ui->actionSave_File_as, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    ui->actionSave_File_as->setShortcut(QKeySequence::SaveAs);
    connect(ui->actionClose, SIGNAL(triggered()),this, SLOT(closeFile()));
    ui->actionClose->setShortcut(QKeySequence::Close);


    connect(ui->actionPreset, SIGNAL(triggered()), this, SLOT(changePreset()));
    connect(ui->menuOpened, SIGNAL(triggered(QAction*)), this, SLOT(switchScene(QAction*)));
    //set icon
    QIcon iconQS(":/image/QScore.jpg");
    setWindowIcon(iconQS);
    //initialize

    resize(defaultWinSize);

    wavView = new WavView(ui->wavTab, wavViewRect.width(),wavViewRect.height());
    wavView->move(0,0);
    scoreView = new ScoreView(ui->scoreTab, scoreViewRect.width(),scoreViewRect.height());
    scoreView->move(0,0);
    staffView = new StaffView(ui->staffTab, staffViewRect.width(),staffViewRect.height());
    staffView->move(0,0);
    keyView = new QSView(ui->wavTab, keyViewRect.width(),keyViewRect.height());//must be created after wavView!
    keyView->move(keyViewRect.x(),keyViewRect.y());
    new KeyScene(keyView);


    addScene(wavView);
    addScene(scoreView);
    addScene(staffView);
    preset = new QSPreset(this);

    ui->statusBar->showMessage("Welcome to QtScoreur!");

    //music play
    playButton = new QPushButton(this);
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->move(0, 400);
    positionSlider = new QSlider(Qt::Horizontal,ui->wavTab);
    positionSlider->resize(wavViewRect.width(),20);
    positionSlider->move(wavViewRect.bottomLeft());
    positionSlider->setRange(0, 0);


    mediaPlayer = new QMediaPlayer(this,QMediaPlayer::VideoSurface);
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this,SLOT(positionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(positionSlider, SIGNAL(valueChanged(int)), this, SLOT(setPosition(int)));
    connect(playButton, SIGNAL(clicked()), this, SLOT(musicPlay()));


}
//music slot:
void QSWindow::musicPlay(){
   if(mediaPlayer->state() ==QMediaPlayer::PlayingState){
       mediaPlayer->pause();
       playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
   }
   else{
       qDebug()<<"media duration:"<<mediaPlayer->duration();
       mediaPlayer->play();
       playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
   }
}
void QSWindow::durationChanged(qint64 duration){
    positionSlider->setRange(0, duration);
    positionSlider->setSingleStep(duration/400.0);
}
void QSWindow::positionChanged(qint64 position){
    positionSlider->setValue(position);
}
void QSWindow::setPosition(int position){
    mediaPlayer->setPosition(position);
    QScrollBar * bar = wavView->horizontalScrollBar();
    bar->setValue(position/(qreal)positionSlider->maximum()*bar->maximum());

}
void QSWindow::mediaStateChanged(QMediaPlayer::State state){
    if(state == QMediaPlayer::StoppedState)
        qDebug()<<"stopped";
    else
        qDebug()<<"start!";
}

QSWindow::~QSWindow()
{ 
    delete mediaPlayer;
    delete ui;
}

/// @brief static member
QSize QSWindow::defaultWinSize = QSize(960,500);
QRect QSWindow::keyViewRect = QRect(0,240, 936,130);
QRect QSWindow::wavViewRect = QRect(0,0, 936,204);
QRect QSWindow::scoreViewRect = QRect(0,0, 800,300);
QRect QSWindow::staffViewRect = QRect(0,0, 800,300);


/// @brief file control module
void QSWindow::openFile(){
    openFileName = QFileDialog::getOpenFileName
            (this, "Open File: ", QDir::homePath(), "*.wav *.txt *.mid");
    if(openFileName.isEmpty())
        ui->statusBar->showMessage("open file unchosen!", 5000);
    else{
        ui->statusBar->showMessage(QString("open: %1").arg(openFileName));
        if(openFileName.endsWith("wav", Qt::CaseInsensitive)){
            addScene(wavView, openFileName);

            mediaPlayer->setMedia(QUrl::fromLocalFile(openFileName));

            playButton->setEnabled(true);
        }
        else if(openFileName.endsWith("txt", Qt::CaseInsensitive))
        {
            addScene(scoreView, openFileName);
        }
        else if(openFileName.endsWith("mid", Qt::CaseInsensitive)){
            addScene(staffView, openFileName);
        }else{
            ui->statusBar->showMessage("can't process such file format!");
            openFileName = "";
        }
    }
}
void QSWindow::saveFile(){
    if(ui->tabWidget->currentWidget() == ui->wavTab)
        saveFileName = QFileDialog::getSaveFileName(this, "Save .wav File: ", QDir::homePath(), "*.wav");
    else if(ui->tabWidget->currentWidget() == ui->scoreTab)
        saveFileName = QFileDialog::getSaveFileName(this, "Save .txt File: ", QDir::homePath(), "*.txt");
    else if(ui->tabWidget->currentWidget() == ui->staffTab)
        saveFileName = QFileDialog::getSaveFileName(this, "Save .mid File: ", QDir::homePath(), "*.mid");

    if(saveFileName.isEmpty())
        ui->statusBar->showMessage("save file name unchosen!", 5000);
    else if(QDir(saveFileName).exists()){
        ui->statusBar->showMessage(QString("the file %1 already exists! Please change to another!").arg(saveFileName),5000);
        saveFileName = "";
    }
    else{
        ui->statusBar->showMessage(QString("save: %1").arg(saveFileName));
    }
}
void QSWindow::saveFileAs(){
    ui->statusBar->showMessage("save as ?");
}

void QSWindow::closeFile(){
    QGraphicsView *view = ui->tabWidget->currentWidget()->findChild
            <QGraphicsView *>(QString(),Qt::FindDirectChildrenOnly);
    //current view
    if(view->scene() == 0){
        ui->statusBar->showMessage("no current file to delete!");
        return;
    }
    QSScene* qs = (QSScene*)view->scene();//current scene
    delete qs;
    qDebug()<<"after delete:"<<view->children().size();//index starts from 4 (3 default children)
    if(view->findChild<QSScene*>(QString(),Qt::FindDirectChildrenOnly) != 0)//auto switch to another file if possible
        emit ((QSScene*)view->findChild<QSScene*>(QString(),Qt::FindDirectChildrenOnly))->Opened()->triggered();
}

// other Slots:

void QSWindow::on_verticalScrollBar_valueChanged(int value)
{
    qDebug()<<value;
}

void QSWindow::addScene(QGraphicsView *view, QString fileName){
    if(view->scene() != 0)
        ((QSScene *)view->scene())->Opened()->setChecked(false);
    if(view == wavView){
        new WavScene(view, fileName);
    }else if(view == scoreView){
        new ScoreScene(view, fileName);
    }else if(view == staffView){
        new StaffScene(view, fileName);
    }
    QSScene* qs = (QSScene*)view->scene();
    ui->menuOpened->addAction(qs->Opened());
    emit qs->Opened()->triggered();
}

void QSWindow::switchScene(QAction * act){
    QSScene *qs = (QSScene *)act->parent();//triggered one
    QSScene *qs1 = (QSScene *)((QGraphicsView *)qs->parent())->scene();//current one
    if(ui->tabWidget->currentWidget() != qs->parent()->parent())//switch tab
        ui->tabWidget->setCurrentWidget((QWidget*)qs->parent()->parent());
    if(qs1 != 0) (qs1)->Opened()->setChecked(false);
    if(qs != qs1) ((QGraphicsView*)qs->parent())->setScene(qs);
    act->setChecked(true);
    ui->statusBar->showMessage(QString("current file: %1").arg(qs->Name()), 4000);
}

void QSWindow::closeEvent(QCloseEvent *){
    bool checked = true;//to check file state
    if( checked == true){
        mediaPlayer->setMedia(QUrl());//discard media to prevent error
        qDebug()<<"discard media and quit";
        QApplication::quit();
    }else{
        ui->statusBar->showMessage("you haven't saved the file(s)!");
    }
}

void QSWindow::changePreset(){
    preset->show();
}


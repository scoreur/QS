#include "qswindow.h"
#include "ui_qswindow.h"
#include "qsview.h"
#include "Wave/wavscene.h"
#include "Score/scorescene.h"
#include "Staff/staffscene.h"
#include "midiparser.h"
#include <QFileDialog>
#include <QFile>
#include <QIcon>
#include <QPushButton>
#include <QSettings>
#include <QDebug>



////////////////////////////////////////////////
/// @brief constructor of QS mainwindow
///
///
///
/// @author wyj
///////////////////////////////////////////////
QSWindow::QSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QSWindow),
    openFileName(""),
    saveFileName(""),
    tempFileName(""),
    mediaPlayer(0),
    musicthread(0),
    preset(QSPreset::getInstance(this))
{
    //qDebug()<<QSettings(domainName, appName).scope();
    //QSettings(domainName, appName).clear();//remove this line in release version
    ui->setupUi(this);
    preset->readSettings();//ALSO construct ALL Views!!

    //set icon
    QIcon iconQS(":/image/QScore.jpg");
    setWindowIcon(iconQS);
    //initialize




    ui->statusBar->showMessage(QString("Welcome to QtScoreur! ")+QDir::currentPath());

    //music play
    playButton = new QPushButton(ui->wavTab);
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->setText("play");
    playButton->move(0, wavView->height()+25);
    positionSlider = new QSlider(Qt::Horizontal,ui->wavTab);
    positionSlider->resize(wavView->width(),20);
    positionSlider->move(wavView->rect().bottomLeft());
    positionSlider->setRange(0, 0);

    musicthread = new QSPlayer(openFileName);                                                                                                               ////
    mediaPlayer = musicthread->player;
    musicthread->start();

    preloadConnect();//holding all connections needed
    addScene(wavView);
    addScene(scoreView);
    addScene(staffView);
    keyScene = new KeyScene(keyView, this);

    //add webView
    webView = new Html5ApplicationViewer(ui->webTab);
    webView->setOrientation(Html5ApplicationViewer::ScreenOrientationAuto);
    webView->resize(900, 400);
    webView->showExpanded();
    //webView->loadUrl(QUrl(QLatin1String("http://www.scoreur.net")));



}
QSWindow::~QSWindow()
{
    //delete mediaPlayer;
    delete ui;
}
////////////////////////////////////////////////////////////////







//////////////////////////////////////////////////////////////////
/// @brief music slot:
void QSWindow::musicPlay(){
    qDebug()<<mediaPlayer->state();
   if(mediaPlayer->state() !=QMediaPlayer::PlayingState){
       mediaPlayer->play();
       playButton->setText("pause");
       playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
   }
   else{
       playButton->setText("pause");
       qDebug()<<"media duration:"<<mediaPlayer->duration();
       mediaPlayer->pause();
       playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
       playButton->setText("play");
   }
}
void QSWindow::durationChanged(qint64 duration){
    //qDebug()<<"duration changed"<<duration;
    positionSlider->setRange(0, duration);
    positionSlider->setSingleStep(duration/800.0);
}
void QSWindow::positionChanged(qint64 position){
    //qDebug()<<"positionChanged"<<position;
    positionSlider->setValue(position);
    QScrollBar * bar = wavView->horizontalScrollBar();
    bar->setValue(position/(qreal)positionSlider->maximum()*bar->maximum());//


}
void QSWindow::setPosition(){
    mediaPlayer->setPosition(positionSlider->value());

}
void QSWindow::mediaStateChanged(QMediaPlayer::State state){
    if(state == QMediaPlayer::StoppedState)
        qDebug()<<"stopped";
    else
        qDebug()<<"start!";
}
/// end of music slot
/////////////////////////////////////////////////////////////

void QSWindow::preloadConnect(){
    //file control buttons and shortcuts
    connect(ui->actionOpen_File, SIGNAL(triggered()), SLOT(openFile()));
    ui->actionOpen_File->setShortcut(QKeySequence::Open);
    connect(ui->actionSave_File, SIGNAL(triggered()), SLOT(saveFile()));
    ui->actionSave_File->setShortcut(QKeySequence::Save);
    connect(ui->actionSave_File_as, SIGNAL(triggered()), SLOT(saveFileAs()));
    ui->actionSave_File_as->setShortcut(QKeySequence::SaveAs);
    connect(ui->actionClose, SIGNAL(triggered()), SLOT(closeFile()));
    ui->actionClose->setShortcut(QKeySequence::Close);

    connect(ui->actionPreset, SIGNAL(triggered()), SLOT(changePreset()));
    connect(ui->actionDisplay_Keyboard, SIGNAL(triggered()), SLOT(displayKeyBoard()));
    connect(ui->menuOpened, SIGNAL(triggered(QAction*)), SLOT(switchScene(QAction*)));


   //music state management
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)),
            this,SLOT(positionChanged(qint64)),Qt::QueuedConnection);
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)),
            this, SLOT(durationChanged(qint64)),Qt::QueuedConnection);
    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this,
            SLOT(mediaStateChanged(QMediaPlayer::State)),Qt::QueuedConnection);
    connect(positionSlider, SIGNAL(sliderReleased()), this, SLOT(setPosition()),Qt::QueuedConnection);
    connect(playButton, SIGNAL(clicked()), this, SLOT(musicPlay()),Qt::QueuedConnection);
}



void QSWindow::generalPreset(int mode, int arg){

    switch(mode){
        qDebug()<<arg;
    case 1:


    case 2:

    case 3:

        break;

    default: break;
    }

}
void QSWindow::generalPreset(int mode, QSize size){

    switch(mode){
    case 1:
        resize(size);
        break;

    case 2:
        ui->tabWidget->resize(size);
        break;
    default:
        break;
    }
}

///

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief file control module
void QSWindow::openFile(){                                                                                                  ////
    openFileName = QFileDialog::getOpenFileName                                                                             ////
            (this, "Open File: ", QDir::homePath(), "*.wav *.txt *.mid");                                                   ////
    if(openFileName.isEmpty())                                                                                              ////
        ui->statusBar->showMessage("open file unchosen!", 5000);                                                            ////
    else{                                                                                                                   ////
        ui->statusBar->showMessage(QString("open: %1").arg(openFileName));                                                  ////
        if(openFileName.endsWith("wav", Qt::CaseInsensitive)){                                                              ////
            addScene(wavView, openFileName);                                                                                ////
            mediaPlayer->setMedia(QUrl::fromLocalFile(openFileName));
            playButton->setEnabled(true);
            //mediaPlayer->moveToThread(musicThread);

        }
        else if(openFileName.endsWith("txt", Qt::CaseInsensitive))
        {
            addScene(scoreView, openFileName);
        }
        else if(openFileName.endsWith("mid", Qt::CaseInsensitive)){
            addScene(staffView, openFileName);
            MidiParser::test(openFileName.toStdString());
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
        ui->statusBar->showMessage("no current file to delete!");                                                           ////
        return;                                                                                                             ////
    }                                                                                                                       ////
    QSScene* qs = (QSScene*)view->scene();//current scene                                                                   ////
    delete qs;                                                                                                              ////
    qDebug()<<"after delete:"<<view->children().size();//index starts from 4 (3 default children)                           ////
    if(view->findChild<QSScene*>(QString(),Qt::FindDirectChildrenOnly) != 0)//auto switch to another file if possible       ////
        emit ((QSScene*)view->findChild<QSScene*>(QString(),Qt::FindDirectChildrenOnly))->Opened()->triggered();            ////
}                                                                                                                           ////
/// end of file control module                                                                                              ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






/// @brief other Slots:

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
    }//now the current scene was set to the new one
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
    qs->views()[0]->setToolTip(QDir(qs->Name()).dirName());

    ui->statusBar->showMessage(QString("current file: %1").arg(qs->Name()), 4000);
    if(qs->parent() == ui->wavTab && QDir(qs->Name()).exists())
        mediaPlayer->setMedia(QUrl::fromLocalFile(qs->Name()));
}

void QSWindow::displayKeyBoard(){
    if(keyView->isVisible()){
        keyView->hide();
        ui->actionDisplay_Keyboard->setText("Show Keyboard");
    }else{
        keyView->show();
        ui->actionDisplay_Keyboard->setText("Hide Keyboard");
    }
}

void QSWindow::keyInput(quint8 id, quint8 dura = 12){
    if(dura == 0) id = 88;
    if(ui->tabWidget->currentWidget()==ui->scoreTab)
        ui->scoreInput->insertPlainText(QString::number(id)+":"+QString::number(dura)+"; ");
}
void QSWindow::keyPressEvent(QKeyEvent *event){
    if(keyView->isVisible())
        keyScene->keyPressEvent(event);

}
void QSWindow::keyReleaseEvent(QKeyEvent *event){
    if(keyView->isVisible())
        keyScene->keyReleaseEvent(event);
}

void QSWindow::closeEvent(QCloseEvent *event){
    bool checked = false;//to check file state
    if(ui->scoreInput->toPlainText().isEmpty())
        checked = true;
    if( checked == true){

        musicthread->quit();
        delete musicthread;
        //preset->writeSettings();
        //qDebug()<<"store the size info";
        event->accept();
    }else{
        ui->statusBar->showMessage("you haven't saved the file(s)! Please empty the textbox!");
        ui->tabWidget->setCurrentWidget(ui->scoreTab);
        event->ignore();
    }
}

void QSWindow::changePreset(){
    preset->show();

}


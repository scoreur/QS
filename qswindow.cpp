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
#include <QDebug>
//#include <QSoundEffect>
/*
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
    ~musicPlay(){}
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
*/

/// @brief constructor of QS mainwindow
QSWindow::QSWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QSWindow),
    openFileName(""),
    saveFileName(""),
    tempFileName("")
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
    connect(ui->menuOpened, SIGNAL(triggered(QAction*)), this, SLOT(switchScene(QAction*)));
    //set icon
    QIcon iconQS(":/image/QScore.jpg");
    setWindowIcon(iconQS);
    //initialize

    keyView = new QSView(ui->wavTab, 936,130);
    keyView->move(0,204);
    keyScene = new KeyScene(keyView);//parent not keyView

    wavView = new WavView(ui->wavTab, 936,204);
    wavView->move(0,0);
    scoreView = new ScoreView(ui->scoreTab, 800,300);
    scoreView->move(0,0);
    staffView = new StaffView(ui->staffTab, 800,300);
    staffView->move(0,0);

    addScene(wavView);
    addScene(scoreView);
    addScene(staffView);
    preset = new QSPreset(this);

    ui->statusBar->showMessage("Welcome to QtScoreur!");

}

QSWindow::~QSWindow()
{ 
    delete preset;
    delete ui;
}

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
            //musicthread = new musicPlay(QUrl(QString("file://") + openFileName));
            //musicthread->start();
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

void QSWindow::changePreset(){
    preset->show();
}


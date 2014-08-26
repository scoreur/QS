#include "qspreset.h"
#include "ui_qspreset.h"
#include "qswindow.h"
#include "ui_qswindow.h"
#include "Score/scoreitem.h"
#include "Score/scorescene.h"

#include <QString>
#include <QDebug>
#include <QSettings>

QSPreset::QSPreset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSPreset)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);

    widgetConsturct();

}

QSPreset::~QSPreset(){
    delete ui;
    qDebug()<<"preset dialog delete!";
}




void QSPreset::widgetConsturct(){
    QSettings settings(domainName, appName);

    ui->listWidget->addItem("general");
    ui->listWidget->addItem("wave");
    ui->listWidget->addItem("score");
    ui->listWidget->addItem("staff");
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)),
            ui->stackedWidget, SLOT(setCurrentIndex(int)));

    //general
    settings.beginGroup("general");
    ui->editWinWidth->setText(QString::number(settings.value("winSize", winSize).toSize().width()));
    ui->editWinHeight->setText(QString::number(settings.value("winSize", winSize).toSize().height()));
    ui->editTabWidth->setText(QString::number(settings.value("tabSize", tabSize).toSize().width()));
    ui->editTabHeight->setText(QString::number(settings.value("tabSize", tabSize).toSize().height()));
    settings.endGroup();


    //wave


    //score
    ui->editNoteSpacing->setText(QString::number(uint(ScoreItem::notespacing)));
    ui->editLineSpacing->setText(QString::number(uint(ScoreItem::linespacing)));
    ui->editNoteSize->setText(QString::number(uint(ScoreItem::notesize)));


    //staff




}

void QSPreset::accept(){
    QSWindow * win = (QSWindow*)parent();
    ScoreScene * scoreScene = (ScoreScene*)win->scoreView->scene();
    switch(ui->stackedWidget->currentIndex()){
    case 0://ui->generalPreset:
        win->resize(QSize(ui->editWinWidth->text().toInt(),
                                           ui->editWinHeight->text().toInt()));
        win->generalPreset(2,QSize(ui->editTabWidth->text().toInt(),
                                                      ui->editTabHeight->text().toInt()));

        writeSettings();

        break;

    case 1://ui->wavPreset:
        break;

    case 2://ui->scorePreset:

        ScoreItem::notespacing = ui->editNoteSpacing->text().toInt(0,10);
        ScoreItem::linespacing =  ui->editLineSpacing->text().toInt(0,10);
        ScoreItem::notesize = ui->editNoteSize->text().toInt(0,10);
        scoreScene->lineUpdate();
        scoreScene->update(scoreScene->sceneRect());
        break;
    case 3://ui->staffPreset:
        break;
    default:
        break;
    }

    hide();

}


void QSPreset::reject(){
    qDebug()<<"preset unchanged!";
    hide();
}

//////////////////////////////////////////////////////////////////////////////
/// @brief static member
//general                                                ////
QSize QSPreset::winSize = QSize(960,500);
QSize QSPreset::tabSize = QSize(940,400);                                 ////
QRect QSPreset::keyViewRect = QRect(0,300, 936,130);                      ////
QRect QSPreset::wavViewRect = QRect(0,0, 936,204);                        ////
QRect QSPreset::scoreViewRect = QRect(0,0, 770,300);                      ////
QRect QSPreset::staffViewRect = QRect(0,0, 800,300);

//wave

//score
QSize QSPreset::scorePageSize = QSize(800,600);
QRect QSPreset::scorePaddingRect = QRect(10,10,
                                         QSPreset::scorePageSize.width()-20,
                                         QSPreset::scorePageSize.height()-20);
QColor QSPreset::wavForegroundColor = QColor(20, 175, 50, 220);
QColor QSPreset::wavBackgroundColor = QColor(15, 15, 15, 220);
//staff

///                                                                       ////
//////////////////////////////////////////////////////////////////////////////

void QSPreset::readSettings(){
    QSettings settings(domainName, appName);
    QSWindow * win = (QSWindow*)parent();
    settings.beginGroup("general");
    win->resize(settings.value("winSize", winSize).toSize());
    win->ui->tabWidget->resize(settings.value("tabSize", tabSize).toSize());
    settings.endGroup();

    win->wavView = (new WavView(win->ui->wavTab, wavViewRect.width(),wavViewRect.height()));
    win->scoreView = (new ScoreView(win->ui->scoreTab, scoreViewRect.width(),scoreViewRect.height()));
    win->staffView = (new StaffView(win->ui->staffTab, staffViewRect.width(),staffViewRect.height()));
    win->keyView = (new QSView(win->ui->centralWidget, keyViewRect.width(),keyViewRect.height()));

    win->wavView->move(0,0);
    win->scoreView->move(0,0);
    win->staffView->move(0,0);
    win->keyView->move(keyViewRect.x(),keyViewRect.y());
}
void QSPreset::writeSettings(){
    QSettings settings(domainName, appName);
    QSWindow * win = (QSWindow*)parent();
    settings.beginGroup("general");
    settings.setValue("winSize", win->size());
    settings.setValue("tabSize", win->ui->tabWidget->size());
    settings.endGroup();
}


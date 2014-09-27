#include "qspreset.h"
#include "ui_qspreset.h"
#include "General/qswindow.h"
#include "ui_qswindow.h"
#include "Score/scoreitem.h"
#include "Score/scorescene.h"
#include <QDir>
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
    updateImages();
    updatePictures();

}
QSPreset * QSPreset::instance = 0;
QSPreset::~QSPreset(){
    delete ui;
    if(instance==this) instance = 0;
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
//general
QSize QSPreset::winSize = QSize(960,500);
QSize QSPreset::tabSize = QSize(940,400);                                 ////
QRect QSPreset::keyViewRect = QRect(0,300, 936,130);                      ////
QRect QSPreset::wavViewRect = QRect(0,0, 936,204);                        ////
QRect QSPreset::scoreViewRect = QRect(0,0, 770,300);                      ////
QRect QSPreset::staffViewRect = QRect(0,0, 800,300);

//wave
QColor QSPreset::wavForegroundColor = QColor(20, 200, 50, 220);
QColor QSPreset::wavBackgroundColor = QColor(30, 30, 30, 240);

//score
QSize QSPreset::scorePageSize = QSize(800,600);
QRect QSPreset::scorePaddingRect = QRect(20,20,
                                         QSPreset::scorePageSize.width()-40,
                                         QSPreset::scorePageSize.height()-40);

//staff
QSize QSPreset::staffPageSize = QSize(800, 600);
QRectF QSPreset::staffPaddingRect = QRect(10, 10,
                                                QSPreset::staffPageSize.width()-20,
                                                QSPreset::staffPageSize.height()-20);
QSize QSPreset::staffLineSize = QSize(750, 50);
qreal QSPreset::staffLineInterval = 9;
qreal QSPreset::staffLineSpacing = 50;
QColor QSPreset::noteColor[2] = {QColor(15,15,15,250), Qt::blue};


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

//loading static images
void QSPreset::updateImages(){
    int tmp = 70;
    images[tr_clef] = QPixmap(QString(":/image/staff/treble_clef.png"), "PNG").scaled(70, 70);
    images[ba_clef] = QPixmap(QString(":/image/staff/bass_clef"), "PNG").scaled(45, 45);
    images[qut_rest] = QPixmap(QString(":/image/staff/quarter_rest"), "PNG").scaled(tmp, tmp);
    images[quv_rest] = QPixmap(QString(":/image/staff/quaver_rest"), "PNG").scaled(tmp, tmp);
    images[sharp] = QPixmap(QString(":/image/staff/sharp.png"), "PNG").scaled(25, 25);
    images[flat] = QPixmap(QString(":/image/staff/flat.png"), "PNG").scaled(25, 25);
    images[nature] = QPixmap(QString(":/image/staff/sharp.png"), "PNG").scaled(tmp, tmp);// TODO: replace by nature.png

}
//loading dynamic pictures
void QSPreset::updatePictures(){
    QPainter painter; QPen pen(QSPreset::noteColor[0]); QBrush brush(QSPreset::noteColor[0]);
    qreal shx = 0, shy = -0.35; qreal intv = QSPreset::staffLineInterval;

    QPicture picture;
    painter.begin(&picture);//hollow_note_head
    painter.setPen(pen);
    painter.shear(shx,shy);
    painter.drawEllipse(QPointF(0,0), intv*0.65,intv*0.45);
    painter.end();
    picture.save(QDir::homePath().append("/black_hollow_note.pic"));//test
    painter.begin(&picture);//solid_note_head; TODO: allow multi-color
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.shear(shx,shy);
    painter.drawEllipse(QPointF(0,0), intv *0.65, intv*0.45);
    painter.end();
    picture.save(QDir::homePath().append("/black_solid_note.pic"));


}


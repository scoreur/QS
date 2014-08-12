#include "qspreset.h"
#include "ui_qspreset.h"
#include "Score/scoreitem.h"
#include "qswindow.h"
#include <QString>
#include <QDebug>

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


    ui->listWidget->addItem("general");
    ui->listWidget->addItem("wave");
    ui->listWidget->addItem("score");
    ui->listWidget->addItem("staff");
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)),
            ui->stackedWidget, SLOT(setCurrentIndex(int)));

    //general
    ui->editWinWidth->setText(QString::number(QSWindow::defaultWinSize.width()));
    ui->editWinHeight->setText(QString::number(QSWindow::defaultWinSize.height()));
    ui->editTabWidth->setText(QString::number(QSWindow::defaultTabSize.width()));
    ui->editTabHeight->setText(QString::number(QSWindow::defaultTabSize.height()));


    //wave


    //score
    ui->editNoteSpacing->setText(QString::number(uint(ScoreItem::notespacing)));
    ui->editLineSpacing->setText(QString::number(uint(ScoreItem::linespacing)));
    ui->editNoteSize->setText(QString::number(uint(ScoreItem::notesize)));


    //staff




}

void QSPreset::accept(){
    switch(ui->stackedWidget->currentIndex()){
    case 0://ui->generalPreset:
        ((QSWindow *)parent())->resize(QSize(ui->editWinWidth->text().toInt(),
                                           ui->editWinHeight->text().toInt()));
        ((QSWindow *)parent())->generalPreset(2,QSize(ui->editTabWidth->text().toInt(),
                                                      ui->editTabHeight->text().toInt()));

        break;

    case 1://ui->wavPreset:
        break;

    case 2://ui->scorePreset:
        ScoreItem::scorePreset(1, ui->editNoteSpacing->text().toInt(0,10));
        ScoreItem::scorePreset(2, ui->editLineSpacing->text().toInt(0,10));
        ScoreItem::scorePreset(3, ui->editNoteSize->text().toInt(0,10));

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

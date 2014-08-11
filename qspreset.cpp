#include "qspreset.h"
#include "ui_qspreset.h"
#include "Score/scoreitem.h"
#include <QString>
#include <QDebug>

QSPreset::QSPreset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSPreset)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    ui->editNoteSpacing->setText(QString::number(uint(ScoreItem::notespacing)));
    ui->editLineSpacing->setText(QString::number(uint(ScoreItem::linespacing)));
    ui->editNoteSize->setText(QString::number(uint(ScoreItem::notesize)));
}

QSPreset::~QSPreset(){
    delete ui;
    qDebug()<<"preset dialog delete!";
}

void QSPreset::accept(){
    switch(ui->stackedWidget->currentIndex()){
    case 0://ui->generalPreset:

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
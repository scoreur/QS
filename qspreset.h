#ifndef QSPRESET_H
#define QSPRESET_H

#include <QDialog>

namespace Ui{
class QSPreset;
}

class QSPreset : public QDialog
{
    Q_OBJECT
public:
    explicit QSPreset(QWidget *parent = 0);
    ~QSPreset();

signals:

public slots:
    void accept();
    void reject();
private:
    Ui::QSPreset *ui;

private slots:


};

#endif // QSPRESET_H

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

    //general(in QSWindow)
    static QSize winSize;
    static QSize tabSize;
    static QRect keyViewRect;
    static QRect wavViewRect;
    static QRect scoreViewRect;
    static QRect staffViewRect;
    //wave
    static QColor wavForegroundColor;
    static QColor wavBackgroundColor;

    //score
    static QSize scorePageSize;
    static QRect scorePaddingRect;

    //staff


    void readSettings();
    void writeSettings();

public slots:
    void accept();
    void reject();

private:
    Ui::QSPreset *ui;
    void widgetConsturct();
    //void switchStack(QWidget *widget);

private slots:


};

#endif // QSPRESET_H

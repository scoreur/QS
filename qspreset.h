#ifndef QSPRESET_H
#define QSPRESET_H

#include <QDialog>
#include <QPicture>

class StaffImages{// holding images for painting staves
public:
    StaffImages();// loading&scaling images for qrc
    // TODO: use sigleton pattern
    ~StaffImages(){

    }
    enum TYPE{
        tr_clef = 0,
        ba_clef,
        qut_rest,
        quv_rest,
        sharp,
        flat,
        nature,

        half_note_head = 100,
        qut_note_head
    };
    QPixmap images[20];//TODO: add more
    QPixmap &operator[](quint32 i){
            return images[i%20];
    }
    QPicture &operator()(quint32 i){
        return pic[i%10];
    }
    QPicture pic[10];

    void updateImages();
    void updatePictures();

};

enum LineType{
    NOTES = 0,
    TITLE,
    AUTHOR,
    COMMENT,
    LYRIC,
    FOOTTER
};

namespace Ui{
class QSPreset;
}


class QSPreset : public QDialog
{
    Q_OBJECT
public:

    ~QSPreset();
    static QSPreset* getInstance(QWidget *parent = 0){
        if(instance == 0)
            instance = new QSPreset(parent);
        return instance;
    }

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
    static QSize staffPageSize;
    static QRectF staffPaddingRect;
    static QSize staffLineSize;
    static qreal staffLineInterval;
    static qreal staffLineSpacing;
    static QColor noteColor[2];
    StaffImages imgs;


    void readSettings();
    void writeSettings();

public slots:
    void accept();
    void reject();

private:
    explicit QSPreset(QWidget *parent = 0);
    Ui::QSPreset *ui;
    static QSPreset *instance;
    void widgetConsturct();
    //void switchStack(QWidget *widget);

private slots:

};


#endif // QSPRESET_H

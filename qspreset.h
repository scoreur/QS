/////////////////////////////////
/// @file qspreset.h
///
/// @brief This file manages various preset parameters/resources for QS project
///
////////////////////////////////

#ifndef QSPRESET_H
#define QSPRESET_H

#include <QDialog>
#include <QPicture>



namespace Ui{
class QSPreset;
}

/**
 * @brief The QSPreset class
 *
 *
 */

class QSPreset : public QDialog
{
    Q_OBJECT
public:
    ~QSPreset();
    /**
     * @brief getInstance
     * @param parent
     * @return the singleton instance of QSPreset
     */
    static QSPreset* getInstance(QWidget *parent = 0){
        if(instance == 0) instance = new QSPreset(parent);
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

    /**
     * @brief The StaffType enum
     *
     * Define the type of staff notes
     */
    enum StaffType{
        tr_clef = 0,
        ba_clef,
        qut_rest,
        quv_rest,
        sharp,
        flat,
        nature,

    };
    /**
     * @brief store the staff images
     */
    QPixmap images[20];//TODO: add more
    /**
     * @brief operator []
     * @param i
     * @return the image of staff item with defined enum index
     */
    QPixmap &operator[](quint32 i){
            return images[i%20];
    }
    /**
     * @brief update staff images
     */
    void updateImages();
    /**
     * @brief update QPicture with painter commands
     */
    void updatePictures();

    /**
     * @brief read Settings of last storage
     */
    void readSettings();
    /**
     * @brief save Settings when exit the program
     */
    void writeSettings();

public slots:
    void accept();
    void reject();

private:
    explicit QSPreset(QWidget *parent = 0);
    Ui::QSPreset *ui;
    static QSPreset *instance;
    /**
     * @brief Consturct the stack widget of preset dialog
     */
    void widgetConsturct();
    //void switchStack(QWidget *widget);

private slots:

};

// types of ScoreLine & StaffLine
/**
 * @brief The LineType enum
 *
 * Define supported line type in scoreView and staffView
 */
enum LineType{
    NOTES = 0,
    TITLE,
    AUTHOR,
    COMMENT,
    LYRIC,
    FOOTTER
};


#endif // QSPRESET_H

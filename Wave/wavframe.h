#ifndef WAVFRAME_H
#define WAVFRAME_H
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QPicture>
#include <string>
#include "wavfile.h"

/**
 * @brief The WavFrame class, for playing waveform clips
 */
class WavFrame: public QGraphicsItem{

public:
    WavFrame(quint32 _datasize, short *_data, qreal _intv, qreal _width, qreal halfheight, quint8 step=1);
    ~WavFrame(){
        delete []data;
    };

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath shape() const;
    QRectF boundingRect() const{
        return bound;
    }

    /**
     * @brief process wave data for construction waveform picture
     * @param _data
     * @param num
     * @param step
     */
    void processData(qint16 *_data, quint32 num, quint8 step=1);
    /**
     * @brief construction waveform picture
     */
    void constructPic();
    /**
     * @brief update waveform as music playing
     * @param _data
     * @param num
     * @param step
     */
    void fresh(qint16 _data[], quint32 num, quint8 step=1);
    static quint16 framesize;
    /**
     * @brief useInterpolation3P, whether to use interpolation for fine looking wave curve
     */
    static bool useInterpolation3P;

signals:

public slots:

private:

    quint32 datasize;
    qreal interval;//the horizontal difference between neighbor points
    qreal amplitude;
    QPointF *data;
    //QVector<QPointF>pdata;
    QRectF bound;
    QPicture pic;

};



#endif // WAVFRAME_H

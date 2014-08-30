#ifndef WAVFRAME_H
#define WAVFRAME_H
#include <QPainter>
#include <QGraphicsItem>
#include <string>
#include <fstream>

class WavFrame: public QGraphicsItem{

public:
    WavFrame(quint32 _datasize, short *_data, qreal _intv, qreal _width, qreal _amp);
    ~WavFrame(){
        delete []data;
    };

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPainterPath shape() const;
    QRectF boundingRect() const{
        return bound;
    }

    static quint16 framesize;
signals:

public slots:

private:

    quint32 datasize;
    qreal interval;//the horizontal difference between neighbor points
    qreal amplitude;
    QPointF *data;
    QVector<QPointF>pdata;
    QRectF bound;

};

const quint8 default_wavHeader[44] = {
    82, 73, 70, 70,      // 'RIFF'
    255, 255, 255, 255,  // dataSize+36
    87, 65, 86, 69,      // 'WAVE'
    102, 109, 116, 32,   // 'fmt '
    16, 0, 0, 0,         // wavSize:16||18
    1, 0, 1, 0,          // pcm:1, nchannels
    68, 172, 0, 0,       // sampleps:44100=68+172*256
    16, 177, 2, 0,       // Bps(BytesPerSecond)=sampleps*blockAlign
    2, 0, 16, 0,         // BlockAlign, bpsample
    100, 97, 116, 97,    // Bpsample
    255, 255, 255, 255
};

class WavFile{
public:
    WavFile(quint32 _len=0):data(0), length(_len){
        quint32 dataSize = length*2;
        memcpy(header, default_wavHeader, 44);
        memcpy(header+40, &dataSize, 4);
        dataSize += 36;
        memcpy(header+4, &dataSize, 4);
        if(length>0)
            data = new qint16[length];
    }
    ~WavFile(){
        if(length>0)
            delete []data;
    }

    void save(const char *fileName) const{
        std::ofstream out(fileName);
        out.write((char*)header, 44);
        out.write((char*)data, length*2);
    }
    quint16 nChannels() const{
        return (quint16)(header[22]);
    }
    bool setChannels(quint16 n){
        if(n==1 || n==2){
            header[22] = (quint8)n;
            header[32] = n*2;
            return true;
        }else
            return false;//not supported
    }
    quint16 sampleps() const{
        return *(quint16*)(header+24);
    }

    bool setSampleps(quint16 n){
        if(n==44100 || n==22050 || n==48000){
            memcpy(header+24, &n, 2);
            return true;
        }else
            return false;//not supported


    }


    bool addSingleTone(qreal secs, qreal freq, std::vector<quint8>overtone,
                       quint16 amp=4000, bool overwrite=true, qreal offsecs=0);
    bool addChord(qreal secs, qreal freq, std::vector<qreal>chord, quint16 amp=4000,
                  bool overwrite=true, qreal offsecs=0);
    bool addArpeggio(qreal secs, qreal freq, std::vector<qreal>chord, quint16 amp=4000,
                     bool overwrite=true, qreal offsecs=0, quint8 loop=1);
    bool addSilence(qreal secs, qreal offsecs=0);
    bool addFilter(qreal secs, qreal offsecs, qreal *filter, quint32 l);
    quint8 header[44];
    qint16 *data;
    quint32 length;


    static void test();

};


#endif // WAVFRAME_H

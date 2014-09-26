#ifndef WAVFILE_H
#define WAVFILE_H
#include <string>
#include <QDebug>;

class QSWindow;



class WavFile{
public:
    WavFile(quint32 _len=0):data(0), length(0){
        memcpy(header, default_wavHeader, 44);
        if(setLength(_len))
            data = new qint16[length];
    }
    WavFile(const WavFile &wavFile):length(wavFile.length),
        data(new qint16[wavFile.length]){
        memcpy(header, wavFile.header, 44);
        memcpy((char*)data, (char*)wavFile.data, wavFile.length*2);
    }

    WavFile(const char* fileName):WavFile((quint32)0){
        if(! load(fileName))
             qDebug()<<"wavFile load failed!";
    }

    ~WavFile(){
        if(length>0)
            delete []data;
    }
    enum chordName{
        chordM=0,
        chordm,
        chorddim,
        chordaug,
        chordMaj7,
        chordM7,
        chordm7,
        //inversion
        chordM_1,
        chordM_2,
        chordm_1,
        chordm_2,
        chordMaj7_1,
        chordMaj7_2,
        chordMaj7_3,
        chordM7_1,
        chordM7_2,
        chordM7_3,
        chordm7_1,
        chordm7_2,
        chordm7_3
    };

    static quint32 df_chord[12];
    static std::vector<qreal> chordGen(const char *d);

    bool save(const char *fileName) const;
    bool load(const char *fileName);

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
    bool setLength(quint32 _len){
        bool tmp = (length < _len);
        length = _len;
        quint32 dataSize = length*2;
        memcpy(header+40, &dataSize, 4);
        dataSize += 36;
        memcpy(header+4, &dataSize, 4);
        return tmp;// need to expand data[] or not
    }

    bool addSingleTone(qreal secs, qreal freq, std::vector<quint8>overtone,
                       quint16 amp=4000, bool overwrite=true, qreal offsecs=0);
    bool addChord(qreal secs, qreal freq, std::vector<qreal>chord, quint16 amp=4000,
                  bool overwrite=true, qreal offsecs=0);
    bool addArpeggio(qreal secs, qreal freq, std::vector<qreal>chord, quint16 amp=4000,
                     bool overwrite=true, qreal offsecs=0, quint8 loop=1);
    bool addSilence(qreal secs, qreal offsecs=0);
    bool addFilter(qreal secs, qreal offsecs, qreal *filter, quint32 l);

    bool fromScore(qreal secs, quint8 *score, quint8 *dura, quint32 l);
    bool fromScore(qreal secs, const char* fileName);
    bool fromPcm(quint32 channellen, qint16 *pcm_l, qint16 *pcm_r=0, bool append=false);
    qreal getDuration() const{
        return length/nChannels()/(qreal)sampleps();
    }
    quint8 header[44];
    qint16 *data;
    quint32 length;

    static bool keysoundGen(const QString &filePath, qreal secs);
    static void test();
    int from_lame(const QString &in, const QString &out);
    static void from_lame0(QSWindow *win, const QString &in, const QString &out);

    static qint16 maxAmp;
    static qreal overtone_decr[16];
    static qreal envelope_flute(qreal d);
    static quint8 default_wavHeader[44];
    static char chords[][4];
};

#endif // WAVFILE_H

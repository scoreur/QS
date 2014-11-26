#ifndef WAVFILE_H
#define WAVFILE_H
#include <string>
#include <QDebug>;

class QSWindow;


/**
 * @brief The WavFile class, handling I/O operations of wave file
 */
class WavFile{
public:
    /**
     * @brief default constructor of WavFile
     * @param _len
     */
    WavFile(quint32 _len=0):data(0), length(0){
        memcpy(header, default_wavHeader, 44);
        if(setLength(_len))
            data = new qint16[length];
    }
    /**
     * @brief copy constructor of WavFile
     * @param wavFile
     */
    WavFile(const WavFile &wavFile):length(wavFile.length),
        data(new qint16[wavFile.length]){
        memcpy(header, wavFile.header, 44);
        memcpy((char*)data, (char*)wavFile.data, wavFile.length*2);
    }

    /**
     * @brief construct WavFile from .wav file
     * @param fileName
     */
    WavFile(const char* fileName):WavFile((quint32)0){
        if(! load(fileName))
             qDebug()<<"wavFile load failed!";
    }

    ~WavFile(){
        if(length>0)
            delete []data;
    }
    /**
     * @brief The chordName enum, supported chords type
     */
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

    /**
     * @brief save wavd data into .wav file
     * @param fileName
     * @return
     */
    bool save(const char *fileName) const;
    /**
     * @brief load wave data from .wav file
     * @param fileName
     * @return
     */
    bool load(const char *fileName);

    /**
     * @brief nChannels
     * @return number of channels, 1 or 2
     */
    quint16 nChannels() const{
        return (quint16)(header[22]);
    }
    /**
     * @brief setChannels
     * @param n
     * @return whether the operation is successful
     */
    bool setChannels(quint16 n){
        if(n==1 || n==2){
            header[22] = (quint8)n;
            header[32] = n*2;
            return true;
        }else
            return false;//not supported
    }
    /**
     * @brief sampleps
     * @return sample rate of the wave file
     */
    quint16 sampleps() const{
        return *(quint16*)(header+24);
    }
    /**
     * @brief setSampleps
     * @param n
     * @return
     */
    bool setSampleps(quint16 n){
        if(n==44100 || n==22050 || n==48000){
            memcpy(header+24, &n, 2);
            return true;
        }else
            return false;//not supported
    }
    /**
     * @brief setLength
     * @param _len
     * @return
     */
    bool setLength(quint32 _len){
        bool tmp = (length < _len);
        length = _len;
        quint32 dataSize = length*2;
        memcpy(header+40, &dataSize, 4);
        dataSize += 36;
        memcpy(header+4, &dataSize, 4);
        return tmp;// need to expand data[] or not
    }

    /**
     * @brief addSingleTone, generate single tone of sine form, with given overtones and amplitudes
     * @param secs
     * @param freq
     * @param overtone
     * @param amp
     * @param overwrite
     * @param offsecs
     * @return
     */
    bool addSingleTone(qreal secs, qreal freq, std::vector<quint8>overtone,
                       quint16 amp=4000, bool overwrite=true, qreal offsecs=0);
    /**
     * @brief addChord, add related tones simultaneously forming the given chord
     * @param secs
     * @param freq
     * @param chord
     * @param amp
     * @param overwrite
     * @param offsecs
     * @return
     */
    bool addChord(qreal secs, qreal freq, std::vector<qreal>chord, quint16 amp=4000,
                  bool overwrite=true, qreal offsecs=0);
    /**
     * @brief addArpeggio, add related tones in a sequence forming an arpeggio
     * @param secs
     * @param freq
     * @param chord
     * @param amp
     * @param overwrite
     * @param offsecs
     * @param loop
     * @return
     */
    bool addArpeggio(qreal secs, qreal freq, std::vector<qreal>chord, quint16 amp=4000,
                     bool overwrite=true, qreal offsecs=0, quint8 loop=1);
    /**
     * @brief addSilence, create silence at desired position of desired length
     * @param secs
     * @param offsecs
     * @return
     */
    bool addSilence(qreal secs, qreal offsecs=0);
    /**
     * @brief addFilter, applying given filter
     * @param secs
     * @param offsecs
     * @param filter
     * @param l
     * @return
     */
    bool addFilter(qreal secs, qreal offsecs, qreal *filter, quint32 l);

    /**
     * @brief fromScore, synthesize the wave data from score data
     * @param secs
     * @param score
     * @param dura
     * @param l
     * @return
     */
    bool fromScore(qreal secs, quint8 *score, quint8 *dura, quint32 l);
    /**
     * @brief fromScore, synthesize the wave data from score file
     * @param secs
     * @param fileName
     * @return
     */
    bool fromScore(qreal secs, const char* fileName);
    /**
     * @brief fromPcm, asigned format to raw wave data to form valid .wav file
     * @param channellen
     * @param pcm_l
     * @param pcm_r
     * @param append
     * @return
     */
    bool fromPcm(quint32 channellen, qint16 *pcm_l, qint16 *pcm_r=0, bool append=false);
    /**
     * @brief getDuration
     * @return the duration of the wave file in seconds
     */
    qreal getDuration() const{
        return length/nChannels()/(qreal)sampleps();
    }
    quint8 header[44];
    qint16 *data;
    quint32 length;

    static bool keysoundGen(const QString &filePath, qreal secs);
    static void test();
    /**
     * @brief from_lame, using libmp3lame to decode mp3
     * @param in
     * @param out
     * @return
     */
    int from_lame(const QString &in, const QString &out);
    /**
     * @brief from_lame0, static version of mp3 decoding for multithread usage
     * @param win
     * @param in
     * @param out
     */
    static void from_lame0(QSWindow *win, const QString &in, const QString &out);

    static qint16 maxAmp;
    static qreal overtone_decr[16];
    static qreal envelope_flute(qreal d);
    static quint8 default_wavHeader[44];
    static char chords[][4];
};

#endif // WAVFILE_H

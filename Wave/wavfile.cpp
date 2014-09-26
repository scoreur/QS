#include "wavfile.h"
#include "Core/spectrum.h"
#include "lame.h"
#include <fstream>
#include <QDir>
#include <exception>
#include "General/qswindow.h"
#include "ui_qswindow.h"

//static members
qint16 WavFile::maxAmp = 1<<15;

qreal WavFile::overtone_decr[16] = {0,1, 0.5, 0.3, 0.2, 0.2, 0.1, 0.1, 0.08, 0.06,0.05,
                           0.04,0.03,0.02,0.01,0};

char WavFile::chords[][4] = { "43" // major
                          ,"34" // minor
                          ,"33" // dim
                          ,"44" // aug
                          ,"434"// major7
                          ,"433"// (dom)7
                          ,"343"// minor7
                           //inversion
                          ,"35","54"
                          ,"45","53"
                          ,"341","414","143"
                          ,"332","324","243"
                          ,"432","323","234"
};

quint8 WavFile::default_wavHeader[44] = {
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
    255, 255, 255, 255   // dataSize
};
std::vector<qreal> chordGen(quint8 a, quint8 b, quint8 c=0){
    std::vector<qreal> tmp;
    tmp.push_back(::pow(2.0,a/12.0));
    tmp.push_back(::pow(2.0,b/12.0));
    if(!c)
        tmp.push_back(::pow(2.0,c/12.0));
    return tmp;
}

std::vector<qreal> WavFile::chordGen(const char *d){
    std::vector<qreal> tmp;
    quint8 i=0; qreal start=1;
    while(d[i]!='\0'){
        start *= ::pow(2.0,(d[i]-'0')/12.0);
        tmp.push_back(start);
        ++i;
    }
    return tmp;
}
const qreal thres1 = 0.1, thres2=0.2, qk = 0.5/(1-thres2-thres1), qdis=(1.5+qk*thres1);
qreal WavFile::envelope_flute(qreal d){
    d = d-(qint16)d;
    if(d<thres1){
        qreal t= d/thres1;
        return 1.5*t*t*t;
                //(1-::cos(d/thres1*M_PI)
    }
    else if(d<1-thres2)
        return qdis-qk*d;
    else
        return (1-::cos((1-d)/thres2*M_PI))/2;
}
qreal envelope_piano(qreal d){
    return 1;
}

bool WavFile::addSingleTone(qreal secs, qreal freq, std::vector<quint8> overtone, quint16 amp, bool overwrite, qreal offsecs){
    quint32 samplesize = (quint32)(secs*sampleps());
    quint8 step = nChannels();
    quint32 start = (quint32)(sampleps()*offsecs);
    if(length/step<start) return false;
    if((start+samplesize)*step>length) samplesize = length/step-start;

    qreal wfr = freq = (2*M_PI)/sampleps()*freq;
    if(overwrite){

        for(quint32 i=0,j=step*start;i<samplesize;++i,j+=step)
            data[j] = amp*envelope_flute(i/(qreal)samplesize)*::sin(wfr*i);
    }else
        for(quint32 i=0,j=step*start;i<samplesize;++i,j+=step)
            data[j] += amp*envelope_flute(i/(qreal)samplesize)*::sin(wfr*i);
    for(std::vector<quint8>::iterator iter=overtone.begin();
        iter != overtone.end(); ++iter){
        wfr = freq * (*iter); qreal am = amp*overtone_decr[*iter>15?0:*iter];
        for(quint32 i=0,j=step*start;i<samplesize;++i,j+=step)
            data[j] += am*envelope_flute(i/(qreal)samplesize)*::sin(wfr*i);
    }
    if(step == 2)
        for(quint32 i=1,j=start*2+1;i<samplesize;++i,j+=2)
            data[j] = data[j-1];
    return true;

}
bool WavFile::addChord(qreal secs, qreal freq, std::vector<qreal> chord,
                       quint16 amp, bool overwrite, qreal offsecs){
    quint8 o[4]={2,3,4,6};
    std::vector<quint8>overtone(o,o+4);
    qreal fr = freq;
    addSingleTone(secs, fr,  overtone, amp, overwrite, offsecs);
    for(std::vector<qreal>::iterator iter=chord.begin();
        iter != chord.end(); ++iter){
        if(*iter<1) continue;
        qreal fr = *iter * freq;
        addSingleTone(secs, fr, overtone, amp, false, offsecs);
    }
    return true;

}
bool WavFile::addArpeggio(qreal secs, qreal freq, std::vector<qreal> chord, quint16 amp,
                          bool overwrite, qreal offsecs, quint8 loop){
    quint8 o[4]={2,3,4,6};
    std::vector<quint8>overtone(o,o+4);
    qreal fr = freq;

    qreal overlap = secs*0.08;
    for(quint8 k=0;k<loop;++k)
        addSingleTone(secs+overlap, fr, overtone, amp, overwrite, offsecs+secs*k*(chord.size()+1));

    for(std::vector<qreal>::iterator iter=chord.begin();
        iter != chord.end();++iter){
        offsecs += secs;
        fr = *iter * freq;
        for(quint8 k=0; k<loop; ++k)
            addSingleTone(secs+overlap, fr, overtone, amp, overwrite, offsecs+secs*k*(chord.size()+1));
    }
    return true;
}
bool WavFile::addSilence(qreal secs, qreal offsecs){
    quint32 samplesize = (quint32)(secs*sampleps());
    quint8 step = nChannels();
    quint32 start = (quint32)(sampleps()*offsecs);
    if(length/step<start) return false;
    if((start+samplesize)*step>length) samplesize = length/step-start;

    for(quint32 i=0, j=start*step; i<samplesize; ++i,j+=step){
        data[j] = 0;
    }
    if(step==2){
        for(quint32 i=0, j=start*2+1; i<samplesize; ++i,j+=2){
            data[j] = 0;
        }
    }
}


bool WavFile::addFilter(qreal secs, qreal offsecs, qreal *filter, quint32 l){
    quint32 samplesize = (quint32)(secs*sampleps());
    quint8 step = nChannels();
    quint32 start = (quint32)(sampleps()*offsecs);
    if(length/step<start+l) return false;
    if((start+samplesize+l)*step>length) samplesize = length/step-start-l;
    qreal t = 0; qreal nor=0;
    for(quint32 i=0;i<l;++i)
        nor+= (filter[i]*filter[i]);
    nor = ::sqrt(nor);
    qDebug()<<nor;
    for(quint32 i = samplesize-1+start+l-1,j=i*step; i>=start+l-1; --i,j-=step){
        t = data[j]*filter[0];
        for(quint32 k=1; k<l; ++k){
            t += filter[k]*data[j-step*k];//convolution
        }
        data[j] = (qint16)(t/nor);
    }
    if(step==2)
        for(quint32 i = samplesize-1+start+l-1,j=i*2+1; i>=start+l-1; --i,j-=2){
            t = data[j]*filter[0];
            for(quint32 k=1; k<l; ++k){
                t += filter[k]*data[j-2*k];//convolution
            }
            data[j] = (qint16)(t/nor);
        }
    return true;

}
bool WavFile::fromScore(qreal secs, quint8 *score, quint8 *dura, quint32 l){
    if(length>0) delete[]data;
    quint32 count=0;
    for(quint32 i=0;i<l;++i)
        count += dura[i];
    count/=12;
    qDebug()<<"time: "<<count*secs;
    length = nChannels()*(quint32)(count*secs*sampleps());
    quint32 dataSize = length*2;
    memcpy(header+40, &dataSize, 4);
    dataSize += 36;
    memcpy(header+4, &dataSize, 4);
    data = new qint16[length];
    qreal offsecs = 0;
    quint8 o[4]={2,3,4,6};
    std::vector<quint8>overtone(o,o+4);
    for(quint32 i=0;i<l;++i){
        if(!addSingleTone(dura[i]*secs/12.0, FreqPiano[score[i]], overtone, 4000, true, offsecs)){
            qDebug()<<"failed: "<<i;
            return false;
        }
        offsecs+=(dura[i]*secs/12.0);
    }
    return true;
}
bool WavFile::fromScore(qreal secs, const char *fileName){
    std::fstream scoreIn;
    scoreIn.open(fileName, std::ios::in);
    if(!scoreIn) return false;
    quint16 tmp=0, i=0;
    std::vector<quint8> score, dura;
    while(!scoreIn.eof()){
        scoreIn>>tmp;
        score.push_back((quint8)tmp);
        scoreIn>>tmp;
        dura.push_back((quint8)tmp);
        ++i;
    }
    qDebug()<<"score load success!";
    scoreIn.close();
    return fromScore(secs, (quint8*)&score[0],(quint8*)&dura[0],(quint32)i);
}

bool WavFile::fromPcm(quint32 channellen, qint16 *pcm_l, qint16 *pcm_r, bool append){
    quint8 step = (pcm_r==0)? 1:2;
    setChannels((quint16)step);
    if(length>0) delete[]data;
    if(setLength(channellen*step))
        data = new qint16[length];
    if(step==1)
        memcpy(data, pcm_l, channellen*2);
    else
        for(quint32 i=0;i<channellen;++i){
            data[i*2] = pcm_l[i];
            data[i*2+1] = pcm_r[i];
    }
    return true;
}

bool WavFile::load(const char *fileName){
    std::fstream in; in.clear();
    in.open(fileName, std::ios::in|std::ios::binary);
    if(!in){
        qDebug()<<"file open error";
        return false;
    }
    in.read((char*)header, 36);
    quint32 wavSize = *(quint32*)(header+16);
    if(wavSize != 16)// ==18, skip addpack (2bytes)
        in.seekg(38,std::ios::beg);
    header[16] = (quint8)16;
    in.read((char*)(header+36), 8);
    quint32 datasize = *(quint32*)(header+40);

    if(length>0) delete[]data;

    length = datasize/2;

    data = new qint16[length];

    qDebug()<<length;
    try{
        if(length<100000000000)
            in.read((char*)data, length*2);
        else{
            in.read((char *)data, length/4);
            for(int i=1;i<7;++i){
                in.seekg(length/4*i+44, std::ios::beg);
                in.read((char *)(data+length/8*i), length/4);
            }
        }
        qDebug()<<"first";
        //in.read((char *)(data+length/2), length/2);

        in.seekg(0, std::ios::end);
        qDebug()<<"eof: "<<(int)in.tellg()-44;

    }catch(std::exception &ex){
        qDebug()<<ex.what();
        in.close();
        return false;
    }
    qDebug()<<"close input wav";
    in.close();

    return true;

}
bool WavFile::save(const char *fileName) const{
    std::ofstream out(fileName);
    out.write((char*)header, 44);
    out.write((char*)data, length*2);
    out.close();
    return true;
}

quint32 WavFile::df_chord[12] = {chordM,0,chordm,0,chordm,chordM,0,chordM,0,chordm,0,chorddim};

quint16 biquad(quint8 frnum){
    if(frnum<10) return 10000;
    else if(frnum<20) return 6000;
    else if(frnum<65) return 4000;
    else if(frnum<80) return 3000;
    else return 2000;

}

bool WavFile::keysoundGen(const QString &filePath, qreal secs){
    if(! QDir(filePath).exists()) return false;
    WavFile wavOut(secs*44100);
    quint8 o[4]={2,3,5,7};
    std::vector<quint8>overtone(o,o+4);
    for(quint16 i=0;i<88;++i){
        wavOut.addSingleTone(secs, FreqPiano[i], overtone, biquad((quint8)i));
        wavOut.save((filePath+QString("/piano_%1.wav")).arg(QString::number(i,10)).toStdString().c_str());
    }
    return true;
}

void WavFile::test(){
    qDebug() << "mkdir: "<<QDir(QDir::homePath().append("/Documents")).mkdir(QString("QS_tmp"));
    QString fileName = QDir::homePath().append("/Documents/QS_tmp/arp%1.wav");
    WavFile wavOut(44100 * 2);
    wavOut.addSilence(2);
    qreal fi[10]={0.5,0,0.1,-0.1,0.1,0.5,-0.3,-0.4,0.1,-0.1};
    qreal fil[100];
    for(int i=0;i<100;++i)
        fil[i] = (100-i)/100.0*fi[i%10]*::sin(i*2*M_PI/100);
    //wavOut.addChord(2,FreqPiano[39], chordGen(chords[chordM7]),2000);
    //wavOut.save(fileName.arg(QString("noFilter")).toStdString().c_str());
    //wavOut.addFilter(1,0,fil,100);
    //wavOut.save(fileName.arg(QString("testFilter")).toStdString().c_str());

    WavFile scoreToWav;
    qDebug()<<scoreToWav.getDuration()<<"s";
    scoreToWav.fromScore(60/80.0,"/Users/user/Documents/QS_tmp/thu_anthem_short.txt");
    quint8 frnum[100]={39,44,36,41,46,
                       44,39,41,36,46};
    //quint32 chnum[100]={chordM,chordM,chordm,chordM,chordM};
    quint8 du[100]={2,2,2,1,1,
                   2,2,2,1,1};
    qreal offsecs = 0;
    for(quint16 i=0;i<10;++i){
        //scoreToWav.addChord(60/88.0*du[i], FreqPiano[frnum[i]-12], chordGen(chords[df_chord[frnum[i]%12]]), 2000, false, offsecs);
        offsecs += 60/80.0*du[i];
    }
    //scoreToWav.addFilter(40,0,fil,90);
    scoreToWav.save("/Users/user/Documents/QS_tmp/thu_anthem_short.wav");
}

void lame_display(mp3data_struct *md){//display header info
    qDebug()<<"nchannels:" << md->stereo;
    qDebug()<<"samplerate:" << md->samplerate;
    qDebug()<<"bitrate:" << md->bitrate;
    qDebug()<<"framesize:" << md->framesize;
}
const int buffersize = 4000;
int WavFile::from_lame(const QString &in, const QString &out){
       std::ifstream wavIn(in.toStdString().c_str());
       if(!wavIn){
           qDebug()<<"open file %s error!"<<in;
           return 1;
       }
       std::ofstream wavOut(out.toStdString().c_str());

       mp3data_struct md;
       hip_t mfile = hip_decode_init();

       unsigned char mbuffer[buffersize];
       short pcm_l[40000]; short pcm_r[40000];
       wavIn.read((char*)mbuffer, buffersize);

       int nout = hip_decode_headers( mfile, mbuffer, buffersize, pcm_l, pcm_r, &md);

       lame_display(&md);
       if(md.samplerate != 44100) return 2;
       if(md.bitrate != 128 && md.bitrate != 192) return 3;
       setChannels(md.stereo);
       setSampleps(md.samplerate);
       int i=0;
       wavOut.seekp(44, std::ios::beg);
       for(i=0; i<nout; ++i){
           wavOut.write((char*)(pcm_l+i),2);
           wavOut.write((char*)(pcm_r+i), 2);
       }

       int totallen = nout;
       while( wavIn.read((char*)mbuffer, buffersize)){
           nout = hip_decode(mfile, mbuffer, buffersize, pcm_l, pcm_r);
           qDebug()<<"out:"<<nout;
           if(nout == -1) { qDebug()<<"ERROR!"; hip_decode_exit(mfile); return 3;}
           totallen += nout;
           for(i=0; i<nout; ++i){
               wavOut.write((char*)(pcm_l+i),2);
               wavOut.write((char*)(pcm_r+i), 2);
           }
       }
       qDebug()<<"Exit: /total lenth sec:"<<hip_decode_exit(mfile)<<totallen/44100.0;
       setLength(totallen * nChannels());
       wavOut.seekp(0, std::ios::beg);
       wavOut.write((char*)header, 44);
       wavIn.close();
       wavOut.close();
       return 0;

}
void WavFile::from_lame0(QSWindow *win, const QString &in, const QString &out){
    WavFile wm;
    if( wm.from_lame(in, out) == 0)
        emit win->addFromLameSignal(out);
    else
        qDebug()<<"failed to convert mp3";

}



#include "../qspreset.h"
#include "wavframe.h"
#include <QDebug>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "../Core/spectrum.h"

double ampExpand(double in){
    int sgn = in>0? 1: -1;
    in /= (1<<15);
    //return sgn * ::pow(in*in, 0.2);
    return in;
}

WavFrame::WavFrame(quint32 _len, short *_data, qreal _intv, qreal _width, qreal _amp):
    datasize((quint32)(_width/_intv)),
    interval(_intv), amplitude(_amp),
    bound(QRectF(0,-_amp, _width,_amp*2.0)){

    data = new QPointF[datasize];
    qreal d = _len/(qreal)datasize;
    data[0]=QPointF(0, -amplitude * ampExpand(_data[0]));
    data[1]=QPointF(d, -amplitude * ampExpand(_data[0]));
    data[datasize-2]=QPointF(_len*interval, -amplitude * ampExpand(_data[datasize-1]));
    data[datasize-2]=QPointF(_len*interval-d, -amplitude * ampExpand(_data[datasize-1]));
    for(quint32 i=2;i<datasize-2;++i){
        quint32 j = (quint32)(i*d+0.5);
        qreal delta = i*d-j;
        data[i] = QPointF(i*d*interval, -amplitude * ampExpand(interpolation3P(delta, _data[j-1],_data[j],_data[j+1])));
        //2 to the 15th power as maximum
    }
    qDebug()<<"array "<<datasize;
    quint32 k = 1;
    while(k<_len-100){
    while(k<_len-1 && (_data[k]-_data[k-1]<=5 || _data[k+1]-_data[k]>=-5)
          &&(_data[k]-_data[k-1]>=-5 || _data[k+1]-_data[k]<=5))++k;
    pdata.push_back(QPointF(k*interval, -amplitude*ampExpand(_data[k]))); ++k;
    }
    qDebug()<<"vector:"<<pdata.size();
}

// static member
quint16 WavFrame::framesize = 200;

void WavFrame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen pen(QSPreset::wavForegroundColor);
    pen.setWidth(1);
    painter->setPen(pen);

    //painter->drawPolyline((QPointF*)pdata.begin(), pdata.size());
    painter->drawPolyline(data, datasize);
}

QPainterPath WavFrame::shape() const{
    QPainterPath path;
    path.addRect(bound);
    return path;
}

const qreal thres1 = 0.1, thres2=0.2, qk = 0.5/(1-thres2-thres1), qdis=(1.5+qk*thres1);
qreal envelope_flute(qreal d){
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

}

qreal overtone_decr[16] = {0,1, 0.5, 0.3, 0.2, 0.1, 0.1, 0.08, 0.07, 0.06,0.05,
                           0.04,0.03,0.02,0.01,0};

std::vector<qreal> chordGen(quint8 a, quint8 b, quint8 c=0){
    std::vector<qreal> tmp;
    tmp.push_back(::pow(2.0,a/12.0));
    tmp.push_back(::pow(2.0,b/12.0));
    if(!c)
        tmp.push_back(::pow(2.0,c/12.0));
    return tmp;
}
std::vector<qreal> chordGen(const char *d){
    std::vector<qreal> tmp;
    quint8 i=0; qreal start=1;
    while(d[i]!='\0'){
        start *= ::pow(2.0,(d[i]-'0')/12.0);
        tmp.push_back(start);
        ++i;
    }
    return tmp;
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

const char chords[][4] = { "43" // major
                          ,"34" // minor
                          ,"33" // dim
                          ,"44" // aug
                          ,"434"// major7
                          ,"433"// (dom)7
                          ,"343"// minor7
                         };
enum chordName{
    chordM=0,
    chordm,
    chorddim,
    chordaug,
    chordMaj7,
    chordM7,
    chordm7
};

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

void WavFile::test(){
    qDebug() << "mkdir: "<<QDir(QDir::homePath().append("/Documents")).mkdir(QString("QS_tmp"));
    QString fileName = QDir::homePath().append("/Documents/QS_tmp/arp%1.wav");
    WavFile wavOut(44100 * 2);
    wavOut.addSilence(2);
    qreal fi[10]={0.5,0,0.1,-0.1,0.4,0.3,-0.3,-0.4,0.1,0.5};
    qreal fil[100];
    for(int i=0;i<100;++i)
        fil[i] = fi[i%10]*::sin(i*2*M_PI/100);
    wavOut.addChord(2,FreqPiano[39], chordGen(chords[chordM7]),2000);
    //wavOut.save(fileName.arg(QString("noFilter")).toStdString().c_str());
    wavOut.addFilter(1,0,fil,100);
    //wavOut.save(fileName.arg(QString("testFilter")).toStdString().c_str());





}

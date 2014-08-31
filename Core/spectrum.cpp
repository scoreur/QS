#include "spectrum.h"
#include <QDebug>
#include "Wave/wavframe.h"


Spectrum::Spectrum(qint16 *data, quint32 num, quint8 mode)
{
    //construct spectrum
    build(data, num, mode);

}

//////////////////////////////////////////////
/// @brief several mathematical tools
///
qreal interpolation3P(qreal delta, qreal y0, qreal y1, qreal y2){
    return 0.5*delta*(y0*(delta-1)+y2*(delta+1))-y1*(delta*delta-1);
}
cmplx Spectrum::exp2iPI(double freq){
    return cmplx(::cos(2*M_PI*freq), ::sin(2*M_PI*freq));
}

quint16 Spectrum::order2(quint32 num){//the smallest index of 2-power no less than num
    quint16 l = 0;
    quint32 x = num;
    while(x & ~1){
        l += 2; x >>= 2;
    }
    l = x+l-1; x=1;
    for(quint16 i = 0; i < l ; ++i){
       if(x & num) return ++l;//
       x<<=1;
    }
    return l;
}
quint32 Spectrum::bitrev(quint32 num, quint16 ord){//reverse bits of number ord
    if(ord == 0 ) return 0;
    quint32 k = 1&num;//rightmost bit of num
    for(int j=1;j<ord;++j){
        k = (k<<1)|((num>>j)&1);
    }
    return k;
}

void cmplx_info(const cmplx &c){
    qreal rl = c.real(), im = c.imag();
    if(rl<1e-6 && rl>-1e-6) rl = 0;
    if(im<1e-6 && im>-1e-6) im = 0;
    //qDebug()<<rl<<"+i*"<<im;
    qDebug()<<::sqrt(rl*rl+im*im);
}
//////////////////////////////////////////////



void Spectrum::build(qint16 *data, quint32 num, quint8 mode, quint32 sampleps){
    quint32 samplesize = 128;
    qreal sampledata = 0;
    cmplx w0;

    if(mode>10) mode = 10;
    switch(mode){
    case 0: //TODO: change to CQT
    {
        const quint8 q = 4;
        val.assign(vallen, cmplx(0));
        for(int j=0;j<vallen;++j){

            qreal abs_i=0, delta_i = q*sampleps/FreqPiano[j]/samplesize;//resampling 4 periods

            w0 = exp2iPI(FreqPiano[j]/sampleps*delta_i);
            if(num < delta_i * samplesize){
                qDebug()<<"samples not enough!";
                return;
            }
            for(qint32 k=samplesize-1;k>=0;--k){
                abs_i = delta_i*k;
                quint32 i=(quint32)(abs_i);
                sampledata = *(data+i);
#ifdef ENABLE_INTERPOLATION
                sampledata = *(data+i+1)*(abs_i-i) + sampledata *(1+i-abs_i);//linear interpolation
#endif
                val[j] = val[j]*w0 +sampledata;
            }
            val[j] *= (2.0 /samplesize);//normalize
        }
    }
        break;
    case 10: //more bins=>CQT;

        break;
    default:
    {
        vallen *= (2*mode+1);
        val.assign(vallen, cmplx(0));
        qreal start = PianoFreqGen(-mode/qreal(2*mode+1)), mult = ::pow(2.0, 1.0/12/(2*mode+1));
        for(quint32 i=0;i<vallen;++i){
            val[i] = STFT1P(data, num, start, false);
            start*=mult;
        }

    }
         break;

    }


}


double Spectrum::PianoFreqGen(qreal pitch){
    return 440.0 * ::pow(2.0, (pitch-48.0)/12.0);
}

cmplx Spectrum::STFT1P(const qint16 data[], quint32 num, qreal freq, bool addWindow){
    cmplx tmp = cmplx(0); qreal sampleps = 44100;
    cmplx w = exp2iPI(freq/sampleps);
    num = sampleps/freq*(quint32)(num*freq/sampleps);//shrink to integral periods
    //qDebug()<<"num: "<<num;

    if(addWindow){
        for(quint32 i=num-1;i>=0;--i)
            tmp = w*tmp+data[i]*0.5;//TODO: define window function;
    }else{
        for(qint32 i=num-1;i>=0;--i)
            tmp = w*tmp+data[i]*1.0;
    }
    tmp *= 2.0/num;
    return tmp;

}

/// @brief Discrete Fourier Transform, no specific size
std::vector<cmplx> Spectrum::DFT(const cmplx data[], quint32 num, bool mode){//test passed
    cmplx w0 = mode? exp2iPI(1.0/num): exp2iPI(-1.0/num);
    cmplx w = cmplx(1.0, 0.0);
    std::vector<cmplx> out(num, cmplx(0));
    for(quint32 j=0;j<num;++j){
        for(int k = num-1; k >= 0; --k){//Horner rule
            out[j] = w*out[j]+data[k];
        }
        w *= w0;
        if(mode) out[j] /= (qreal)num;//normalize
    }
    return out;
}

/// @brief Discrete Consine Transform, for real values
std::vector<qreal> Spectrum::DCT(const qreal data[], quint32 num, bool mode){//DCT-II, usual DCT, not tested
    std::vector<qreal> out (num, 0.0);
    for(quint32 i = 0; i < num; ++i){ 
        for(quint32 k = 0; k < num; --k)
            out[i] += data[k]*::cos(i*(k+0.5)*M_PI/num);//need to be optimized
        if(mode) out[i] /=(qreal)num;

    }

    return out;

}

/// @brief Fast Fourier Transform, expand the size to 2-power
std::vector<cmplx> Spectrum::FFT(const cmplx data[], quint32 num, bool mode){//test passed
    //step1: add 0's and reverse bits (eg. 0001<->1000)
    quint16 ord = order2(num); 
    std::vector<cmplx>out(1<<ord, cmplx(0));
    for(quint32 i=0;i<num;++i)
        out[bitrev(i, ord)] = data[i];//copy
    num = 1<<ord;
    cmplx t, u, w, wm;
    quint32 m = 1;
    //step2: butterfly operation
    for(quint16 s = 1; s <= ord; ++s){ //iterative tree depth
        m = 1<<s;                      //length of each operation group
        wm = mode? exp2iPI(1.0/m): exp2iPI(-1.0/m);           //rotating factor
        for(quint32 k = 0; k < num; k += m)//each layer
        {
            w = cmplx(1.0,0.0);
            for(quint32 j = 0; j< (m>>1); ++j){//butterfly
                t = w * out[k+j+(m>>1)];
                u = out[k+j];
                out[k+j] = u+t;
                out[k+j+(m>>1)] = u-t;
                w *= wm;
            }
        }
    }
    if(mode)// not inverse
        for(quint32 k=0;k<num;++k)
            out[k] /= (qreal)num;//normalize
    return out;
}

std::vector<cmplx> Spectrum::CQT(const qreal data[], quint32 num, bool mode){
    qreal fmin= FreqPiano[0], fmax= FreqPiano[87];

    const quint32 nbins = 88*15;
    const quint8 q = 4;
    std::vector<cmplx> out(nbins,cmplx(0));

    return out;
}

const quint32 testsize = 64;
void Spectrum::test(){
    WavFile wt(2*44100);//single channel
    quint8 o[4]={2,3,4,6};
    std::vector<quint8>overtone(o,o+4);
    //wt.addChord(2.0, FreqPiano[39], WavFile::chordGen(chords[WavFile::chordM]),2000);
    qint16 kt[8000];
    for(quint32 i=0;i<8000;++i)
        kt[i] = 5000*::sin(2*M_PI/44100*FreqPiano[39]*i);
    wt.addSingleTone(2.0, FreqPiano[41], overtone, 4000);
    Spectrum s(wt.data+40000,7000);
    for(quint32 i=0;i<s.vallen;++i){
         qDebug()<<i<<"freq: ";
         cmplx_info(s.val[i]);

   }
    qDebug()<<"accurate: ";
    cmplx_info(STFT1P(wt.data+40000, 1000, FreqPiano[41]));




}

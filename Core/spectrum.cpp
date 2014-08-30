#include "spectrum.h"


qreal interpolation3P(qreal delta, qreal y0, qreal y1, qreal y2){
    return 0.5*delta*(y0*(delta-1)+y2*(delta+1))-y1*(delta*delta-1);
}

Spectrum::Spectrum(qint16 *data, quint32 num)
{
    //construct spectrum
}


double Spectrum::PianoFreqGen(int pitch){
    return 440.0 * ::pow(2.0, (pitch-48.0)/12.0);
}

cmplx Spectrum::exp2iPI(double freq){
    return cmplx(::cos(2*M_PI*freq), ::sin(2*M_PI*freq));
}

quint16 Spectrum::order2(quint32 num){
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
quint32 Spectrum::bitrev(quint32 num, quint16 ord){
    if(ord == 0 ) return 0;
    quint32 k = 1&num;
    for(int j=1;j<ord;++j){
        k = (k<<1)|((num>>j)&1);
    }
    return k;
}

cmplx * Spectrum::DFT(cmplx data[], quint32 num, bool mode){
    cmplx w0 = mode? exp2iPI(1/(double)num): exp2iPI(-1/(double)num);
    cmplx w = cmplx(1.0, 0.0);
    cmplx *out = new cmplx[num];
    for(quint32 j=0;j<(num>>1);++j){//calculate half
        out[j] = 0.0;
        for(int k = num-1; k >= 0; --k){//Horner rule
            out[j] = w*out[j]+data[k];
        }
        w *= w0;
        if(mode) out[j] *= (2.0 /num);//normalize
        out[num-j] = cmplx(out[j].real(), -out[j].imag());//conjugate
    }
    return out;
}
void Spectrum::DFT(Spectrum *spectrum, qint16 *data, quint32 num, quint16 samplesize){//for piano
    cmplx w, w0;
    double sampledata = 0;
    for(int j=0;j<88;++j){
        spectrum->spect[j] = w = w0 = exp2iPI(FreqPiano[j]/44100.0);
        //spectrum->spect[3*j+1] = 0.0; this->spect[3*j+2] = 0.0;
        double delta_i = 8*M_PI/FreqPiano[j]/samplesize;//4 periods
        if(num < delta_i * samplesize) return;
        for(int k=0;k<samplesize;++k){//default, for single tone
            double abs_i = delta_i*k;
            int i=(int)(abs_i);
            sampledata = *(data+i+1)*(abs_i-i) + *(data+i)*(1+i-abs_i);//interpolation
            spectrum->spect[j] += (sampledata * w);
            w *= w0;
        }
        spectrum->spect[j] *= (2.0 /samplesize);//normalize
    }
}

double * Spectrum::DCT(double data[], quint32 num, bool mode){//DCT-II, usual DCT

    double *out = new double[num];
    for(quint32 i = 0; i < num; ++i){
        out[i] = 0.0;
        for(quint32 k = 0; k < num; --k)
            out[i] += data[k]*::cos(i*(k+0.5)*M_PI/num);//need to be optimized
        if(mode) out[i] *= (2.0/num);
        else out[i] *= 0.5;
    }

    return out;

}

cmplx * Spectrum::FFT(cmplx data[], quint32 num, bool mode){
    //step1: add 0's and reverse bits (eg. 0001<->1000)
    quint16 ord = order2(num);
    quint32 n = 1<< ord;
    cmplx *out = new cmplx[n]{0};

    for(int i=0;i<num;++i)
        out[bitrev(i, ord)] = data[i];//copy

    cmplx t;//0.0

    //step2: butterfly operation
    for(quint16 s = 1; s <= ord; ++s){//iterative tree depth
        quint32 m = 1<<s;
        cmplx wm = exp2iPI(1.0/m);//rotating factor
        cmplx w,u;
        for(quint32 k = 0; k < (num>>1); k += m)//each layer
        {
            w = cmplx(1.0,0.0);
            for(int j = 0; j< (m>>1); ++j){//butterfly
                t = w * out[k+j+(m>>1)];
                u = out[k+j];
                out[k+j] = u+t;
                out[k+j+(m>>1)] = u-t;
                w *= wm;
            }
        }
    }
    if(mode)
        for(int k=0;k<num;++k)
            out[k] = out[k] * (2.0/num);//normalize
    else
        for(int k=0;k<num;++k)
            out[k] = out[k] * 0.5;//normalize

    return out;
}

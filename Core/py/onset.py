from __future__ import division
import numpy as np
import wave
from scipy.sparse import hstack, vstack, coo_matrix
from math import *
from FFT import FFT


# author lzy
def wav_in(url):

    f = wave.open(url,"rb")
    params = f.getparams()
    nchannels, sampwidth, framerate, nframes = params[:4]
    
    str_data = f.readframes(nframes)
    f.close()
    
    if sampwidth == 2:
        wave_data = np.fromstring(str_data,dtype = np.short)
    else:
        wave_data = np.fromstring(str_data,dtype = np.int8)
    
    
    wave_data.shape = -1,nchannels
    wave_data = wave_data.T
    time = np.arange(0,nframes)*(1.0/framerate)

    """print str_data[:1000]
        print wave_data[0][:1000]
        print max(wave_data[0])"""
    #len_time = len(time)/2
    #time = time[0:len_time]
    
    #print "time length = ",len(time)
    #print "wave_data[0] length = ",len(wave_data[0])
    
    return [wave_data[0],params]


def max_normalize(x):
	if any(x):
            maxx = max(x)
            return [i/maxx for i in x]
        else:
            return x

# author lyp
class CQT :
    def __init__(self, fmin, fmax, bins, fs, wnd) :
        self.eps = 1e-3
        self.fmin = fmin
        self.fmax = fmax
        self.bins = bins
        self.wnd  = wnd
        self.fs = fs
        self.Q = 1 / (pow(2, 1.0 / bins) - 1)
        K = int(ceil(bins * log(fmax / fmin) / log(2)))
        # print 'K:', K
        self.fftlen = int(pow(2, ceil(log(self.Q * fs / fmin) / log(2))))
        return
        self.ker = []
        print '[CQT] Initializing...', fmin, fmax, bins, fs, self.fftlen, self.Q
        for k in range(K, 0, -1) :
            N = ceil(self.Q * fs / (fmin * pow(2, (k - 1.0) / bins)))
            tmpKer = wnd(N) * np.exp(2 * pi * 1j * self.Q * np.arange(N) / N) / N;
            ker = np.fft.fft(tmpKer, self.fftlen)
            print '[CQT] Running...', k
            ker = np.select([abs(ker) > self.eps], [ker])
            print np.sum(ker > self.eps)
            self.ker += [coo_matrix(ker, dtype = np.complex128)]
        # print 'shape:', hstack(self.ker).tocsc().shape
        self.ker.reverse()
        self.ker = vstack(self.ker).tocsc().transpose().conj() / self.fftlen
        print '[CQT] Initialized OK.'
    
    def fast_pp(self, x) :
        # print self.ker.shape, np.fft.fft(x, self.fftlen).shape
        return (np.fft.fft(x, self.fftlen).reshape(1, self.fftlen) * self.ker)[0]
    
    def fast(self, x) :
        for i in range(3) :
            x = x + x
        x = np.array(x)
        print x.shape
        cq = []
        for k in range(1, int(ceil(self.bins * log(self.fmax / self.fmin) / log(2))) + 1) :
            N = int(ceil(self.Q * self.fs / (self.fmin * pow(2, (k - 1.0) / self.bins))))
            xl = min(len(x), N)
            # print x[:N].shape, (wnd(N) * np.exp(2 * pi * 1j * np.arange(N) / N) / N).shape
            cq += [x[:xl].dot((self.wnd(N, xl) * np.exp(-2 * pi * 1j * self.Q * np.arange(xl) / N) / N)[:xl])]
        
        # return np.array(cq)
        cq = map(abs,cq)
        #print '[CQT] Done.'
        return cq







compress = 1
note = ['C','bD','D','bE','E','F','bG','G','bA','A','bB','B']
dict = {'A':1,'Bb':2,'bB':2,'B':3,'C':-8,'bD':-7,'Db':-7,'D':-6,'bE':-5,'Eb':-5,'E':-4,'F':-3,'bG':-2,'Gb':-2,'G':-1,'bA':0,'Ab':0}

def hamming(length, xl) :
    return 0.54 - 0.46 * np.cos(2 * pi * np.arange(xl) / length)

def Hanning(k,N):
    w = 0.5*(1-cos(2*pi*k/(N-1)))
    return w

def pre_process(signal,length):
    signal = max_normalize(signal)
    i=0
    while signal[i]<0.1 :
        signal[i] = 0
        i += 1
	l = int(len(signal)/compress)
    signal = [signal[compress*i] for i in range(l)]
    signal = signal[:length]
    return signal

def transfer_pitch(num):
    num = int(num)
    l = int((num+8) / 12)
    return note[(num+8) % 12]+str(l)
	

# important part ============================
def onset_svm(inputFile, outputFile, length=200000,compress=1):
    [signal,params] = wav_in(inputFile)
    nchannels, sampwidth, nframerate, nframes = params[:4]
    
    framerate = int(nframerate/compress)
    if length < 100:
        length = len(signal)
    signal = pre_process(signal,length)
    #print "length now:",len(signal)
    #print params

    #signal = [signal[d*i] for i in range(int(len(signal)/d))]

    #constant in onset_detection
    h = int(2**ceil(log(framerate*0.03)/log(2))) # hop size
    #print "h=",h
    print "time interval=",h/framerate,"s"
    N = h #*2 size of the Hanning window
    threshhold1 = 0.5 #standard
    threshhold2 = 0.4 #the lowest requirement for f_onset
    threshhold3 = 0.6 #to compensate for the decline of the requirement for f_onset, the local maximium must be greater than threshhold3
    #parameters for cqt
   
    M = int(len(signal)/h) -1
    print "size of M", M
    L = int(N/2)
    wnd = [Hanning(i,N) for i in range(N)]



    print "calulating S..."  #  S stores the result of STFT of the signal
    #S = [[] for i in range(M)]
    f = [0]*(M+1)
    for m in range(M):
        print m
        S = FFT([i*j for i,j in zip(signal[m*h:m*h+N],wnd)])[1:L]
        #S = drd.fast([i*j for i,j in zip(signal[m*h:m*h+N],wnd)])
        S = map(abs, S[:])
        f[m] = sum(S)
    
    print "finish FFT"
    #S = map(lambda str:map(abs,str),S[:M])
    # power spectrum

    print "calculating f and f_onset"


    '''for m in range(M):
        f[m] = sum(S[m][1:int(len(S[m])/2)]) '''# f[m] is the sum of the magnitude spectrum
    f = max_normalize(f);

    f_onset = [0]*(M+1)
    for m in range(1,M):
        if f[m] != 0.0:
            f_onset[m] = 1-f[m-1]/f[m] #detection funtion
        else:
            f_onset[m] = 0
        

    seq_onset = []        
    for m in range(M):
        if (f_onset[m]>f_onset[m-1]) and (f_onset[m]>f_onset[m+1]) and ((f_onset[m]>threshhold1) or ((f_onset[m]>threshhold2) and (max(f[m-1:m+2])>threshhold3))):
            seq_onset += [m]

	time = [(i+1.0)/framerate*h for i in seq_onset]		
    nKey = len(seq_onset)



    fmin = 32.7
    fmax = 3951
    b_hop = 7 # 31 bins per note, 31 can be replaced by any other odd number. trade-off between effiency and accuracy
    bins = b_hop * 12
    rad = int(b_hop/2)

    drd = CQT(fmin,fmax,bins,framerate,hamming)



    F = [0]*nKey

    for j in range(0,nKey):
        i = seq_onset[j]
        """if (i>=2):
            print i,(i+1.0)/framerate*h
            x1 = drd.fast(signal[int((i-2)*h):int(i*h)])
            y1 = []
            for k in range(1,83):
                y1 += [sum(x1[k*b_hop-rad:k*b_hop+rad+1])/b_hop]
            y1 = max_normalize(y1)
        else :
            y1 = [0]*82"""
        
        if (i<=M-4):
            print i,(i+1.0)/framerate*h
            x2 = drd.fast(signal[int((i+2)*h):int((i+4)*h)])
            y2 = []
            for k in range(1,83):
                y2 += [sum(x2[k*b_hop-rad:k*b_hop+rad+1])/b_hop]
            F[j] = max_normalize(y2)
        else:
            F[j] = [0]*82
			
         #only consider the part after the onset
		
		
    y = [0]*len(F)
    
    fout = open(outputFile, 'w')
    for i in range(len(F)):
        for j in range(len(F[i])):
            fout.write(str(F[i][j])+' ')
        fout.write('\n')
    
    fout.close()
    print "finished"

	
    #modified by WYJ
    for i in range(len(time)):
        print time[i]

    '''
    for i in range(len(y)):
       p_labels[i] = int(p_labels[i])
       str_labels[i] = transfer_pitch(int(p_labels[i]))'''


    return time
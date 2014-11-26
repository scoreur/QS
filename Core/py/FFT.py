import math

#constants in STFT
logKAPPA = 10
KAPPA = 2**logKAPPA
ETA = 2**(logKAPPA-1)

#unit roots
def uRoot(k,n):
	return math.cos(2*math.pi*k/n)+math.sin(2*math.pi*k/n)*1j

#int_log of length
def log(x):
	ans = int(math.ceil(math.log(len(x),2)))
	if 2**ans==2*len(x):
		ans -= 1
	return ans

#the nth slice of length KAPPA in signal
def slice(signal, n):
	return signal[ETA*n:ETA*n+KAPPA]
	
#main FFT process, by recursion
def FFT_fund(x,logDeg):
	if logDeg==0:
		return [x[0]]
	else:
		y_even = FFT_fund(x[0::2],logDeg-1)*2
		y_odd = FFT_fund(x[1::2],logDeg-1)*2
		return [y_even[i]+uRoot(i,2**logDeg)*y_odd[i] for i in range(0,2**logDeg)]


#FFT without POT length constrain
def FFT(*args):
	x = args[0]
	if len(args)>1:
		logDeg = args[1]
	else:
		logDeg = log(x)
	return FFT_fund(x+[0]*(2**logDeg-len(x)),logDeg)

def iFFT(*args):
	x = map(lambda z:z.imag+1j*z.real,args[0])
	if len(args)>1:
		return map(lambda z:z.imag+1j*z.real,FFT(x,args[1]))
	else:
		return map(lambda z:z.imag+1j*z.real,FFT(x))

#short time fourier transform
def STFT(signal, n):
	return map(abs,FFT_fund(slice(signal,n),logKAPPA))[:ETA]

def convolution(x,y):
	n = max(log(x),log(y))
	x_FFT,y_FFT = map(lambda x:FFT(x,n),(x,y))
	return iFFT([i*j/2**n for i,j in zip(x_FFT,y_FFT)])

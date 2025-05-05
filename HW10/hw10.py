import csv
import matplotlib.pyplot as plt
import numpy as np

def plotFFT(t, data1, fig, ax1, ax2, c, title=None):
    dt = (t[-1] - t[0])/len(t)
    Ts = dt; # sampling interval
    Fs = 1/Ts # sample rate
    ts = t #np.arange(0,t[-1],Ts) # time vector
    y = data1 # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    ax1.plot(t,y,color=c)
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),color=c) # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    if title is not None:
        #plt.title(title)
        fig.suptitle(title)

def MAF(t, data1, X):
    newdata = np.zeros(len(data1))
    for i in range(X,len(data1)):
        newdata[i] = sum(data1[i-X:i])/X
    
    return newdata

def IIR(signal, A, B):
    new_average = np.zeros(len(signal))
    for i in range(1, len(signal)):
        new_average[i] = A * new_average[i-1] + B * signal[i]
    
    return new_average

def lowpassFIR(weights, signal):
    assert(len(weights) < len(signal))
    out = np.zeros(len(signal))
    for i in range(len(weights), len(signal)):
        temp = 0
        for j in range(0, len(weights)):
            temp = temp + weights[j]*signal[i-len(weights)+j]
        out[i]=temp
    
    return out

h = [
    0.000000000000000000,
    -0.000011466433343440,
    -0.000048159680438716,
    -0.000070951498745996,
    0.000000000000000000,
    0.000226394896924650,
    0.000570593070542985,
    0.000843882357908127,
    0.000742644459879189,
    -0.000000000000000001,
    -0.001387330856962112,
    -0.002974017320060804,
    -0.003876072294410999,
    -0.003078546062261788,
    0.000000000000000002,
    0.004911281747189231,
    0.009897689392343489,
    0.012239432700612913,
    0.009302643950572093,
    -0.000000000000000005,
    -0.013947257358995015,
    -0.027649479941983943,
    -0.034045985830080311,
    -0.026173578588735643,
    0.000000000000000007,
    0.043288592274982135,
    0.096612134128292462,
    0.148460098539443669,
    0.186178664190551207,
    0.199977588313552862,
    0.186178664190551235,
    0.148460098539443669,
    0.096612134128292462,
    0.043288592274982128,
    0.000000000000000007,
    -0.026173578588735653,
    -0.034045985830080325,
    -0.027649479941983936,
    -0.013947257358995019,
    -0.000000000000000005,
    0.009302643950572094,
    0.012239432700612920,
    0.009897689392343489,
    0.004911281747189235,
    0.000000000000000002,
    -0.003078546062261790,
    -0.003876072294411004,
    -0.002974017320060808,
    -0.001387330856962112,
    -0.000000000000000001,
    0.000742644459879189,
    0.000843882357908127,
    0.000570593070542984,
    0.000226394896924650,
    0.000000000000000000,
    -0.000070951498745996,
    -0.000048159680438716,
    -0.000011466433343440,
    0.000000000000000000,
]
paths = [r'C:\\Users\\03022\\Desktop\\ME433\\HW10\\sigA.csv', r'C:\\Users\\03022\\Desktop\\ME433\\HW10\\sigB.csv', r'C:\\Users\\03022\\Desktop\\ME433\\HW10\\sigC.csv', r'C:\\Users\\03022\\Desktop\\ME433\\HW10\\sigD.csv']
for path in paths:
    #path = r'C:\\Users\\03022\\Desktop\\ME433\\HW10\\sigD.csv'
    t = [] # column 0
    data1 = [] # column 1
    data2 = [] # column 2
    with open(path) as f:
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            data1.append(float(row[1])) # second column
            #data2.append(float(row[2])) # third column

    fig, (ax1, ax2) = plt.subplots(2, 1)
    plotFFT(t=t, data1=data1, fig=fig, ax1=ax1, ax2=ax2, c='black')
    #plotFFT(t=t, data1=MAF(t, data1, 100), fig=fig, ax1=ax1, ax2=ax2, c='r', title='100 points MAF')
    #plotFFT(t=t, data1=IIR(data1, 0.9, 0.1), fig=fig, ax1=ax1, ax2=ax2, c='r', title='A=0.9, B=0.1 IIR')
    plotFFT(t=t, data1=lowpassFIR(weights=h, signal=data1), fig=fig, ax1=ax1, ax2=ax2, c='r', title='Low pass sinc, 59 terms, 100hz cutoff freq, 80hz bandwidth')
    plt.show()
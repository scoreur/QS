import sys
import os
import onset
#from libsvm.python.svmutil import *

if len(sys.argv)<5:
    print "usage: <input wav><output txt> <length> <compress>"
    exit(1)


inputFile = sys.argv[1]
outputFile = sys.argv[2]
length = int(sys.argv[3])
compress = int(sys.argv[4])

print "processing\n"+inputFile

time = onset.onset_svm(inputFile, outputFile, length,compress)


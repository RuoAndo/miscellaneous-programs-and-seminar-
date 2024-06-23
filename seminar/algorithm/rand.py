# coding: UTF-8

import numpy as np
import sys
from numpy.random import *

argvs = sys.argv
argc = len(argvs) 

if (argc != 3): 
    print 'Usage: # python d N for ex, 6 * 1000 / 2 = 3000'
    quit()    

rng = np.random.RandomState(123)

# 6 * 1000 /2 = 3000 tmp

d = int(argvs[1])
N = int(argvs[2])

dN = d * N

SRC = rand(10000) * 100 

src = []
for x in SRC:
    print str(int(x)) 



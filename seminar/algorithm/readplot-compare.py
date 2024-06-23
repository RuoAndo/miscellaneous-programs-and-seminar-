#coding:utf-8
# K-means

import numpy as np
import sys
import matplotlib.pyplot as plt
from scipy.ndimage.interpolation import shift

argvs = sys.argv

if __name__ == "__main__":
    data = np.genfromtxt(argvs[1], delimiter=",")

    number = []
    print len(data)

    counter = 0
    for x in data:
        number.append(counter)
        counter = counter + 1
        
    plt.plot(data, number, "o")

    #plt.subplot(3, 1, 1)
    #plt.subplot(3, 1, 2)
    #plt.plot(data[:,1])

    #plt.show()
    ifn = argvs[1] + ".png"
    plt.savefig(ifn) 

# coding: utf-8
# Matplotlib example

import os
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

#matplotlib inline
import matplotlib.pyplot as plt
import numpy as np

t = np.arange(0.0, 2.0, 0.01)
s = 1 + np.sin(2 * np.pi * t)

print(t)

fig, ax = plt.subplots()
ax.plot(t, s)

ax.set(xlabel='time (s)', ylabel='voltage (mV)',
       title='showing sin wave')
ax.grid()

plt.show()

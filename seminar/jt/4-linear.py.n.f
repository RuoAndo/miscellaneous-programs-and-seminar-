4-linear.py:  1: # coding: utf-8
4-linear.py:  2: 
4-linear.py:  3: # **[MLE-01]** モジュールをインポートします。
4-linear.py:  4: 
4-linear.py:  5: # In[1]:
4-linear.py:  6: 
4-linear.py:  7: 
4-linear.py:  8: import tensorflow as tf
4-linear.py:  9: import numpy as np
4-linear.py: 10: import matplotlib.pyplot as plt
4-linear.py: 11: from numpy.random import multivariate_normal, permutation
4-linear.py: 12: import pandas as pd
4-linear.py: 13: from pandas import DataFrame, Series
4-linear.py: 14: 
4-linear.py: 15: 
4-linear.py: 16: # **[MLE-02]** トレーニングセットのデータを用意します。
4-linear.py: 17: 
4-linear.py: 18: # In[2]:
4-linear.py: 19: 
4-linear.py: 20: 
4-linear.py: 21: np.random.seed(20160512)
4-linear.py: 22: 
4-linear.py: 23: n0, mu0, variance0 = 20, [10, 11], 20
4-linear.py: 24: data0 = multivariate_normal(mu0, np.eye(2)*variance0 ,n0)
4-linear.py: 25: df0 = DataFrame(data0, columns=['x1','x2'])
4-linear.py: 26: df0['t'] = 0
4-linear.py: 27: 
4-linear.py: 28: n1, mu1, variance1 = 15, [18, 20], 22
4-linear.py: 29: data1 = multivariate_normal(mu1, np.eye(2)*variance1 ,n1)
4-linear.py: 30: df1 = DataFrame(data1, columns=['x1','x2'])
4-linear.py: 31: df1['t'] = 1
4-linear.py: 32: 
4-linear.py: 33: df = pd.concat([df0, df1], ignore_index=True)
4-linear.py: 34: train_set = df.reindex(permutation(df.index)).reset_index(drop=True)
4-linear.py: 35: 
4-linear.py: 36: 
4-linear.py: 37: # **[MLE-03]** トレーニングセットのデータの内容を確認します。
4-linear.py: 38: 
4-linear.py: 39: # In[3]:
4-linear.py: 40: 
4-linear.py: 41: 
4-linear.py: 42: train_set
4-linear.py: 43: 
4-linear.py: 44: 
4-linear.py: 45: # **[MLE-04]** (x1, x2) と t を別々に集めたものをNumPyのarrayオブジェクトとして取り出しておきます。
4-linear.py: 46: 
4-linear.py: 47: # In[4]:
4-linear.py: 48: 
4-linear.py: 49: 
4-linear.py: 50: train_x = train_set[['x1','x2']].as_matrix()
4-linear.py: 51: train_t = train_set['t'].as_matrix().reshape([len(train_set), 1])
4-linear.py: 52: 
4-linear.py: 53: 
4-linear.py: 54: # **[MLE-05]** トレーニングセットのデータについて、t=1 である確率を求める計算式 p を用意します。
4-linear.py: 55: 
4-linear.py: 56: # In[5]:
4-linear.py: 57: 
4-linear.py: 58: 
4-linear.py: 59: x = tf.placeholder(tf.float32, [None, 2])
4-linear.py: 60: w = tf.Variable(tf.zeros([2, 1]))
4-linear.py: 61: w0 = tf.Variable(tf.zeros([1]))
4-linear.py: 62: f = tf.matmul(x, w) + w0
4-linear.py: 63: p = tf.sigmoid(f)
4-linear.py: 64: 
4-linear.py: 65: 
4-linear.py: 66: # **[MLE-06]** 誤差関数 loss とトレーニングアルゴリズム train_step を定義します。
4-linear.py: 67: 
4-linear.py: 68: # In[6]:
4-linear.py: 69: 
4-linear.py: 70: 
4-linear.py: 71: t = tf.placeholder(tf.float32, [None, 1])
4-linear.py: 72: loss = -tf.reduce_sum(t*tf.log(p) + (1-t)*tf.log(1-p))
4-linear.py: 73: train_step = tf.train.AdamOptimizer().minimize(loss)
4-linear.py: 74: 
4-linear.py: 75: 
4-linear.py: 76: # **[MLE-07]** 正解率 accuracy を定義します。
4-linear.py: 77: 
4-linear.py: 78: # In[7]:
4-linear.py: 79: 
4-linear.py: 80: 
4-linear.py: 81: correct_prediction = tf.equal(tf.sign(p-0.5), tf.sign(t-0.5))
4-linear.py: 82: accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
4-linear.py: 83: 
4-linear.py: 84: 
4-linear.py: 85: # **[MLE-08]** セッションを用意して、Variableを初期化します。
4-linear.py: 86: 
4-linear.py: 87: # In[8]:
4-linear.py: 88: 
4-linear.py: 89: 
4-linear.py: 90: sess = tf.Session()
4-linear.py: 91: sess.run(tf.initialize_all_variables())
4-linear.py: 92: 
4-linear.py: 93: 
4-linear.py: 94: # **[MLE-09]** 勾配降下法によるパラメーターの最適化を20000回繰り返します。
4-linear.py: 95: 
4-linear.py: 96: # In[9]:
4-linear.py: 97: 
4-linear.py: 98: 
4-linear.py: 99: i = 0
4-linear.py:100: for _ in range(20000):
4-linear.py:101:     i += 1
4-linear.py:102:     sess.run(train_step, feed_dict={x:train_x, t:train_t})
4-linear.py:103:     if i % 2000 == 0:
4-linear.py:104:         loss_val, acc_val = sess.run(
4-linear.py:105:             [loss, accuracy], feed_dict={x:train_x, t:train_t})
4-linear.py:106:         print ('Step: %d, Loss: %f, Accuracy: %f'
4-linear.py:107:                % (i, loss_val, acc_val))
4-linear.py:108: 
4-linear.py:109: 
4-linear.py:110: # **[MLE-10]** この時点のパラメーターの値を取り出します。
4-linear.py:111: 
4-linear.py:112: # In[10]:
4-linear.py:113: 
4-linear.py:114: 
4-linear.py:115: w0_val, w_val = sess.run([w0, w])
4-linear.py:116: w0_val, w1_val, w2_val = w0_val[0], w_val[0][0], w_val[1][0]
4-linear.py:117: print(w0_val, w1_val, w2_val)
4-linear.py:118: 
4-linear.py:119: 
4-linear.py:120: # **[MLE-11]** 取り出したパラメーターの値を用いて、結果をグラフに表示します。
4-linear.py:121: 
4-linear.py:122: # In[11]:
4-linear.py:123: 
4-linear.py:124: 
4-linear.py:125: train_set0 = train_set[train_set['t']==0]
4-linear.py:126: train_set1 = train_set[train_set['t']==1]
4-linear.py:127: 
4-linear.py:128: fig = plt.figure(figsize=(6,6))
4-linear.py:129: subplot = fig.add_subplot(1,1,1)
4-linear.py:130: subplot.set_ylim([0,30])
4-linear.py:131: subplot.set_xlim([0,30])
4-linear.py:132: subplot.scatter(train_set1.x1, train_set1.x2, marker='x')
4-linear.py:133: subplot.scatter(train_set0.x1, train_set0.x2, marker='o')
4-linear.py:134: 
4-linear.py:135: linex = np.linspace(0,30,10)
4-linear.py:136: liney = - (w1_val*linex/w2_val + w0_val/w2_val)
4-linear.py:137: subplot.plot(linex, liney)
4-linear.py:138: 
4-linear.py:139: field = [[(1 / (1 + np.exp(-(w0_val + w1_val*x1 + w2_val*x2))))
4-linear.py:140:           for x1 in np.linspace(0,30,100)]
4-linear.py:141:          for x2 in np.linspace(0,30,100)]
4-linear.py:142: subplot.imshow(field, origin='lower', extent=(0,30,0,30),
4-linear.py:143:                cmap=plt.cm.gray_r, alpha=0.5)
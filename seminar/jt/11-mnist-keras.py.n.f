11-mnist-keras.py:  1: # coding: utf-8
11-mnist-keras.py:  2: 
11-mnist-keras.py:  3: # **[MSE-01]** モジュールをインポートして、乱数のシードを設定します。
11-mnist-keras.py:  4: 
11-mnist-keras.py:  5: # In[1]:
11-mnist-keras.py:  6: 
11-mnist-keras.py:  7: 
11-mnist-keras.py:  8: import tensorflow as tf
11-mnist-keras.py:  9: import numpy as np
11-mnist-keras.py: 10: import matplotlib.pyplot as plt
11-mnist-keras.py: 11: from tensorflow.examples.tutorials.mnist import input_data
11-mnist-keras.py: 12: 
11-mnist-keras.py: 13: np.random.seed(20160604)
11-mnist-keras.py: 14: 
11-mnist-keras.py: 15: 
11-mnist-keras.py: 16: # **[MSE-02]** MNISTのデータセットを用意します。
11-mnist-keras.py: 17: 
11-mnist-keras.py: 18: # In[2]:
11-mnist-keras.py: 19: 
11-mnist-keras.py: 20: 
11-mnist-keras.py: 21: mnist = input_data.read_data_sets("/tmp/data/", one_hot=True)
11-mnist-keras.py: 22: 
11-mnist-keras.py: 23: 
11-mnist-keras.py: 24: # **[MSE-03]** ソフトマックス関数による確率 p の計算式を用意します。
11-mnist-keras.py: 25: 
11-mnist-keras.py: 26: # In[3]:
11-mnist-keras.py: 27: 
11-mnist-keras.py: 28: 
11-mnist-keras.py: 29: x = tf.placeholder(tf.float32, [None, 784])
11-mnist-keras.py: 30: w = tf.Variable(tf.zeros([784, 10]))
11-mnist-keras.py: 31: w0 = tf.Variable(tf.zeros([10]))
11-mnist-keras.py: 32: f = tf.matmul(x, w) + w0
11-mnist-keras.py: 33: p = tf.nn.softmax(f)
11-mnist-keras.py: 34: 
11-mnist-keras.py: 35: 
11-mnist-keras.py: 36: # **[MSE-04]** 誤差関数 loss とトレーニングアルゴリズム train_step を用意します。
11-mnist-keras.py: 37: 
11-mnist-keras.py: 38: # In[4]:
11-mnist-keras.py: 39: 
11-mnist-keras.py: 40: 
11-mnist-keras.py: 41: t = tf.placeholder(tf.float32, [None, 10])
11-mnist-keras.py: 42: loss = -tf.reduce_sum(t * tf.log(p))
11-mnist-keras.py: 43: train_step = tf.train.AdamOptimizer().minimize(loss)
11-mnist-keras.py: 44: 
11-mnist-keras.py: 45: 
11-mnist-keras.py: 46: # **[MSE-05]** 正解率 accuracy を定義します。
11-mnist-keras.py: 47: 
11-mnist-keras.py: 48: # In[5]:
11-mnist-keras.py: 49: 
11-mnist-keras.py: 50: 
11-mnist-keras.py: 51: correct_prediction = tf.equal(tf.argmax(p, 1), tf.argmax(t, 1))
11-mnist-keras.py: 52: accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
11-mnist-keras.py: 53: 
11-mnist-keras.py: 54: 
11-mnist-keras.py: 55: # **[MSE-06]** セッションを用意して、Variableを初期化します。
11-mnist-keras.py: 56: 
11-mnist-keras.py: 57: # In[6]:
11-mnist-keras.py: 58: 
11-mnist-keras.py: 59: 
11-mnist-keras.py: 60: sess = tf.Session()
11-mnist-keras.py: 61: sess.run(tf.initialize_all_variables())
11-mnist-keras.py: 62: 
11-mnist-keras.py: 63: 
11-mnist-keras.py: 64: # **[MSE-07]** パラメーターの最適化を2000回繰り返します。
11-mnist-keras.py: 65: # 
11-mnist-keras.py: 66: # 1回の処理において、トレーニングセットから取り出した100個のデータを用いて、勾配降下法を適用します。
11-mnist-keras.py: 67: # 
11-mnist-keras.py: 68: # 最終的に、テストセットに対して約92%の正解率が得られます。
11-mnist-keras.py: 69: 
11-mnist-keras.py: 70: # In[7]:
11-mnist-keras.py: 71: 
11-mnist-keras.py: 72: 
11-mnist-keras.py: 73: i = 0
11-mnist-keras.py: 74: for _ in range(2000):
11-mnist-keras.py: 75:     i += 1
11-mnist-keras.py: 76:     batch_xs, batch_ts = mnist.train.next_batch(100)
11-mnist-keras.py: 77:     sess.run(train_step, feed_dict={x: batch_xs, t: batch_ts})
11-mnist-keras.py: 78:     if i % 100 == 0:
11-mnist-keras.py: 79:         loss_val, acc_val = sess.run([loss, accuracy],
11-mnist-keras.py: 80:             feed_dict={x:mnist.test.images, t: mnist.test.labels})
11-mnist-keras.py: 81:         print ('Step: %d, Loss: %f, Accuracy: %f'
11-mnist-keras.py: 82:                % (i, loss_val, acc_val))
11-mnist-keras.py: 83: 
11-mnist-keras.py: 84: 
11-mnist-keras.py: 85: # **[MSE-08]** この時点のパラメーターを用いて、テストセットに対する予測を表示します。
11-mnist-keras.py: 86: # 
11-mnist-keras.py: 87: # ここでは、「０」?「９」の数字に対して、正解と不正解の例を３個ずつ表示します。
11-mnist-keras.py: 88: 
11-mnist-keras.py: 89: # In[8]:
11-mnist-keras.py: 90: 
11-mnist-keras.py: 91: 
11-mnist-keras.py: 92: images, labels = mnist.test.images, mnist.test.labels
11-mnist-keras.py: 93: p_val = sess.run(p, feed_dict={x:images, t: labels}) 
11-mnist-keras.py: 94: 
11-mnist-keras.py: 95: fig = plt.figure(figsize=(8,15))
11-mnist-keras.py: 96: for i in range(10):                                                                                                                      
11-mnist-keras.py: 97:     c = 1
11-mnist-keras.py: 98:     for (image, label, pred) in zip(images, labels, p_val):
11-mnist-keras.py: 99:         prediction, actual = np.argmax(pred), np.argmax(label)
11-mnist-keras.py:100:         if prediction != i:
11-mnist-keras.py:101:             continue
11-mnist-keras.py:102:         if (c < 4 and i == actual) or (c >= 4 and i != actual):
11-mnist-keras.py:103:             subplot = fig.add_subplot(10,6,i*6+c)
11-mnist-keras.py:104:             subplot.set_xticks([])
11-mnist-keras.py:105:             subplot.set_yticks([])
11-mnist-keras.py:106:             subplot.set_title('%d / %d' % (prediction, actual))
11-mnist-keras.py:107:             subplot.imshow(image.reshape((28,28)), vmin=0, vmax=1,
11-mnist-keras.py:108:                            cmap=plt.cm.gray_r, interpolation="nearest")
11-mnist-keras.py:109:             c += 1
11-mnist-keras.py:110:             if c > 6:
11-mnist-keras.py:111:                 break
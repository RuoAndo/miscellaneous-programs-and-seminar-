import numpy as np
from keras.models import Sequential
from keras.layers.core import Dense, Activation, Dropout
from keras.optimizers import SGD
from sklearn import datasets
from sklearn.model_selection import train_test_split

from sklearn.datasets.base import get_data_home 
from sklearn.datasets import fetch_mldata

import os
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

np.random.seed(0)
print (get_data_home())
mnist = fetch_mldata('MNIST original')

n = len(mnist.data)
N = 10000  
indices = np.random.permutation(range(n))[:N] 

X = mnist.data[indices]
y = mnist.target[indices]
Y = np.eye(10)[y.astype(int)] 

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, train_size=0.8)

n_in = len(X[0]) 
n_hidden = 200
n_out = len(Y[0])


# 参考: Tensorflowでの実装方法 
#W0 = tf.Variable(tf.truncated_normal([n_in, n_hidden], stddev=0.01))
#b0 = tf.Variable(tf.zeros([n_hidden]))
#h0 = tf.nn.relu(tf.matmul(x, W0) + b0)
#h0_drop = tf.nn.dropout(h0, keep_prob)

model = Sequential()
model.add(Dense(n_hidden, input_dim=n_in))
model.add(Activation('tanh'))
model.add(Dropout(0.5))

model.add(Dense(n_hidden))
model.add(Activation('tanh'))
model.add(Dropout(0.5))

model.add(Dense(n_hidden))
model.add(Activation('tanh'))
model.add(Dropout(0.5))

model.add(Dense(n_out))
model.add(Activation('softmax'))

model.compile(loss='categorical_crossentropy',
              optimizer=SGD(lr=0.01),
              metrics=['accuracy'])

epochs = 150
batch_size = 200

model.fit(X_train, Y_train, epochs=epochs, batch_size=batch_size)

loss_and_metrics = model.evaluate(X_test, Y_test)
print(loss_and_metrics)

import tensorflow as tf
hello = tf.constant('this is test')
sess = tf.Session()
print(sess.run(hello))
exit()

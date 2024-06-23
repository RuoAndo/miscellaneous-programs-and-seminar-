10-mnist-tf.py:  1: # Introductory CNN Model: MNIST Digits
10-mnist-tf.py:  2: # ---------------------------------------
10-mnist-tf.py:  3: #
10-mnist-tf.py:  4: # In this example, we will download the MNIST handwritten
10-mnist-tf.py:  5: # digits and create a simple CNN network to predict the
10-mnist-tf.py:  6: # digit category (0-9)
10-mnist-tf.py:  7: 
10-mnist-tf.py:  8: alting_height * conv2_features
10-mnist-tf.py:  9: full1_weight = tf.Variable(tf.truncated_normal([full1_input_size, fully_connected_size1],
10-mnist-tf.py: 10:                            stddev=0.1, dtype=tf.float32))
10-mnist-tf.py: 11: full1_bias = tf.Variable(tf.truncated_normal([fully_connected_size1], stddev=0.1, dtype=tf.float32))
10-mnist-tf.py: 12: full2_weight = tf.Variable(tf.truncated_normal([fully_connected_size1, target_size],
10-mnist-tf.py: 13:                                                stddev=0.1, dtype=tf.float32))
10-mnist-tf.py: 14: full2_bias = tf.Variable(tf.truncated_normal([target_size], stddev=0.1, dtype=tf.float32))
10-mnist-tf.py: 15: 
10-mnist-tf.py: 16: 
10-mnist-tf.py: 17: # Initialize Model Operations
10-mnist-tf.py: 18: def my_conv_net(conv_input_data):
10-mnist-tf.py: 19:     # First Conv-ReLU-MaxPool Layer
10-mnist-tf.py: 20:     conv1 = tf.nn.conv2d(conv_input_data, conv1_weight, strides=[1, 1, 1, 1], padding='SAME')
10-mnist-tf.py: 21:     relu1 = tf.nn.relu(tf.nn.bias_add(conv1, conv1_bias))
10-mnist-tf.py: 22:     max_pool1 = tf.nn.max_pool(relu1, ksize=[1, max_pool_size1, max_pool_size1, 1],
10-mnist-tf.py: 23:                                strides=[1, max_pool_size1, max_pool_size1, 1], padding='SAME')
10-mnist-tf.py: 24: 
10-mnist-tf.py: 25:     # Second Conv-ReLU-MaxPool Layer
10-mnist-tf.py: 26:     conv2 = tf.nn.conv2d(max_pool1, conv2_weight, strides=[1, 1, 1, 1], padding='SAME')
10-mnist-tf.py: 27:     relu2 = tf.nn.relu(tf.nn.bias_add(conv2, conv2_bias))
10-mnist-tf.py: 28:     max_pool2 = tf.nn.max_pool(relu2, ksize=[1, max_pool_size2, max_pool_size2, 1],
10-mnist-tf.py: 29:                                strides=[1, max_pool_size2, max_pool_size2, 1], padding='SAME')
10-mnist-tf.py: 30: 
10-mnist-tf.py: 31:     # Transform Output into a 1xN layer for next fully connected layer
10-mnist-tf.py: 32:     final_conv_shape = max_pool2.get_shape().as_list()
10-mnist-tf.py: 33:     final_shape = final_conv_shape[1] * final_conv_shape[2] * final_conv_shape[3]
10-mnist-tf.py: 34:     flat_output = tf.reshape(max_pool2, [final_conv_shape[0], final_shape])
10-mnist-tf.py: 35: 
10-mnist-tf.py: 36:     # First Fully Connected Layer
10-mnist-tf.py: 37:     fully_connected1 = tf.nn.relu(tf.add(tf.matmul(flat_output, full1_weight), full1_bias))
10-mnist-tf.py: 38: 
10-mnist-tf.py: 39:     # Second Fully Connected Layer
10-mnist-tf.py: 40:     final_model_output = tf.add(tf.matmul(fully_connected1, full2_weight), full2_bias)
10-mnist-tf.py: 41:     
10-mnist-tf.py: 42:     return final_model_output
10-mnist-tf.py: 43: 
10-mnist-tf.py: 44: model_output = my_conv_net(x_input)
10-mnist-tf.py: 45: test_model_output = my_conv_net(eval_input)
10-mnist-tf.py: 46: 
10-mnist-tf.py: 47: # Declare Loss Function (softmax cross entropy)
10-mnist-tf.py: 48: loss = tf.reduce_mean(tf.nn.sparse_softmax_cross_entropy_with_logits(logits=model_output, labels=y_target))
10-mnist-tf.py: 49: 
10-mnist-tf.py: 50: # Create a prediction function
10-mnist-tf.py: 51: prediction = tf.nn.softmax(model_output)
10-mnist-tf.py: 52: test_prediction = tf.nn.softmax(test_model_output)
10-mnist-tf.py: 53: 
10-mnist-tf.py: 54: 
10-mnist-tf.py: 55: # Create accuracy function
10-mnist-tf.py: 56: def get_accuracy(logits, targets):
10-mnist-tf.py: 57:     batch_predictions = np.argmax(logits, axis=1)
10-mnist-tf.py: 58:     num_correct = np.sum(np.equal(batch_predictions, targets))
10-mnist-tf.py: 59:     return 100. * num_correct/batch_predictions.shape[0]
10-mnist-tf.py: 60: 
10-mnist-tf.py: 61: # Create an optimizer
10-mnist-tf.py: 62: my_optimizer = tf.train.MomentumOptimizer(learning_rate, 0.9)
10-mnist-tf.py: 63: train_step = my_optimizer.minimize(loss)
10-mnist-tf.py: 64: 
10-mnist-tf.py: 65: # Initialize Variables
10-mnist-tf.py: 66: init = tf.global_variables_initializer()
10-mnist-tf.py: 67: sess.run(init)
10-mnist-tf.py: 68: 
10-mnist-tf.py: 69: # Start training loop
10-mnist-tf.py: 70: train_loss = []
10-mnist-tf.py: 71: train_acc = []
10-mnist-tf.py: 72: test_acc = []
10-mnist-tf.py: 73: for i in range(generations):
10-mnist-tf.py: 74:     rand_index = np.random.choice(len(train_xdata), size=batch_size)
10-mnist-tf.py: 75:     rand_x = train_xdata[rand_index]
10-mnist-tf.py: 76:     rand_x = np.expand_dims(rand_x, 3)
10-mnist-tf.py: 77:     rand_y = train_labels[rand_index]
10-mnist-tf.py: 78:     train_dict = {x_input: rand_x, y_target: rand_y}
10-mnist-tf.py: 79:     
10-mnist-tf.py: 80:     sess.run(train_step, feed_dict=train_dict)
10-mnist-tf.py: 81:     temp_train_loss, temp_train_preds = sess.run([loss, prediction], feed_dict=train_dict)
10-mnist-tf.py: 82:     temp_train_acc = get_accuracy(temp_train_preds, rand_y)
10-mnist-tf.py: 83:     
10-mnist-tf.py: 84:     if (i+1) % eval_every == 0:
10-mnist-tf.py: 85:         eval_index = np.random.choice(len(test_xdata), size=evaluation_size)
10-mnist-tf.py: 86:         eval_x = test_xdata[eval_index]
10-mnist-tf.py: 87:         eval_x = np.expand_dims(eval_x, 3)
10-mnist-tf.py: 88:         eval_y = test_labels[eval_index]
10-mnist-tf.py: 89:         test_dict = {eval_input: eval_x, eval_target: eval_y}
10-mnist-tf.py: 90:         test_preds = sess.run(test_prediction, feed_dict=test_dict)
10-mnist-tf.py: 91:         temp_test_acc = get_accuracy(test_preds, eval_y)
10-mnist-tf.py: 92:         
10-mnist-tf.py: 93:         # Record and print results
10-mnist-tf.py: 94:         train_loss.append(temp_train_loss)
10-mnist-tf.py: 95:         train_acc.append(temp_train_acc)
10-mnist-tf.py: 96:         test_acc.append(temp_test_acc)
10-mnist-tf.py: 97:         acc_and_loss = [(i+1), temp_train_loss, temp_train_acc, temp_test_acc]
10-mnist-tf.py: 98:         acc_and_loss = [np.round(x, 2) for x in acc_and_loss]
10-mnist-tf.py: 99:         print('Generation # {}. Train Loss: {:.2f}. Train Acc (Test Acc): {:.2f} ({:.2f})'.format(*acc_and_loss))
10-mnist-tf.py:100:     
10-mnist-tf.py:101:     
10-mnist-tf.py:102: # Matlotlib code to plot the loss and accuracies
10-mnist-tf.py:103: eval_indices = range(0, generations, eval_every)
10-mnist-tf.py:104: # Plot loss over time
10-mnist-tf.py:105: plt.plot(eval_indices, train_loss, 'k-')
10-mnist-tf.py:106: plt.title('Softmax Loss per Generation')
10-mnist-tf.py:107: plt.xlabel('Generation')
10-mnist-tf.py:108: plt.ylabel('Softmax Loss')
10-mnist-tf.py:109: plt.show()
10-mnist-tf.py:110: 
10-mnist-tf.py:111: # Plot train and test accuracy
10-mnist-tf.py:112: plt.plot(eval_indices, train_acc, 'k-', label='Train Set Accuracy')
10-mnist-tf.py:113: plt.plot(eval_indices, test_acc, 'r--', label='Test Set Accuracy')
10-mnist-tf.py:114: plt.title('Train and Test Accuracy')
10-mnist-tf.py:115: plt.xlabel('Generation')
10-mnist-tf.py:116: plt.ylabel('Accuracy')
10-mnist-tf.py:117: plt.legend(loc='lower right')
10-mnist-tf.py:118: plt.show()
10-mnist-tf.py:119: 
10-mnist-tf.py:120: # Plot some samples
10-mnist-tf.py:121: # Plot the 6 of the last batch results:
10-mnist-tf.py:122: actuals = rand_y[0:6]
10-mnist-tf.py:123: predictions = np.argmax(temp_train_preds, axis=1)[0:6]
10-mnist-tf.py:124: images = np.squeeze(rand_x[0:6])
10-mnist-tf.py:125: 
10-mnist-tf.py:126: Nrows = 2
10-mnist-tf.py:127: Ncols = 3
10-mnist-tf.py:128: for i in range(6):
10-mnist-tf.py:129:     plt.subplot(Nrows, Ncols, i+1)
10-mnist-tf.py:130:     plt.imshow(np.reshape(images[i], [28, 28]), cmap='Greys_r')
10-mnist-tf.py:131:     plt.title('Actual: ' + str(actuals[i]) + ' Pred: ' + str(predictions[i]),
10-mnist-tf.py:132:               fontsize=10)
10-mnist-tf.py:133:     frame = plt.gca()
10-mnist-tf.py:134:     frame.axes.get_xaxis().set_visible(False)
10-mnist-tf.py:135:     frame.axes.get_yaxis().set_visible(False)
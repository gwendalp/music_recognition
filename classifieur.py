# -*- coding: utf-8 -*-
"""gwen_estimator.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/16_sA2Nj3P3zQYrpxOWUiHC8_x5j6l7L-
"""

import sys


#from google.colab import drive
# drive.mount('/content/gdrive')

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import LabelEncoder, RobustScaler
from sklearn.model_selection import train_test_split, GridSearchCV
from sklearn.metrics import plot_confusion_matrix
from sklearn.svm import LinearSVC
from sklearn.neural_network import MLPClassifier

import tensorflow.keras as keras
import tensorflow as tf
from keras.models import Sequential
from keras.layers import Dense
import os

def SVM():

    IN_COLAB = False
    if IN_COLAB:
        #DATASET_PATH = '/content/gdrive/My Drive/Colab Notebooks/BE_IA_EMBARQUEE/dataset/'
        PATH = '/content/gdrive/My Drive/BE_IA_EMBARQUEE/features_cpp.csv' #'/content/gdrive/My Drive/BE_IA_EMBARQUEE/features_cpp.csv'
    else:
        IN_COLAB = False
        #DATASET_PATH = r'./dataset/'
        PATH = r'./data/features_cpp.csv'

    dataset = pd.read_csv(PATH,sep=';')
    columns = dataset.columns.tolist() # get the columns
    cols_to_use = columns[:len(columns)-1] # drop the last one
    dataset = pd.read_csv(PATH, usecols=cols_to_use,sep=";")

    print(dataset)

    batch_audio = pd.DataFrame(dataset).to_numpy()

    print(batch_audio)


    features = batch_audio[:, 1:]
    y = batch_audio[:, 0]

    print(y)



    X_train, X_test, y_train, y_test = train_test_split(features, y, test_size=0.20, random_state=0)
    #X_train_, X_valid, y_train_, y_valid = train_test_split(X_train, y_train, test_size=0.20, random_state=0)
    # NORMALISATION
    rbs_clf = RobustScaler()
    X_train = rbs_clf.fit_transform(X_train)
    X_test = rbs_clf.transform(X_test)

    print("X_train :", X_train.shape)
    print("y_train :", y_train.shape)
    print("X_test :", X_test.shape)
    print("y_test :", y_test.shape)

    """# **Non optimisé**"""

    model = LinearSVC(C=0.1, max_iter=1e5, tol=1e-4)
    print(model.fit(X_train, y_train))
    print(model.score(X_test, y_test))

    """# **Optimisé**"""

    param_grid  = {
                        'C': [0.001, 0.01, 0.1, 1, 10],
                        'loss': ['squared_hinge', 'hinge'],
                        'tol': [1e-4, 1e-5, 1e-3, 1e-6],
                        'max_iter': [ 1e5, 1e6]
                    }
    model = LinearSVC()
    # model.fit(X_train, y_train)

    grid = GridSearchCV(model, param_grid ,  cv=3)

    print(grid.fit(X_train, y_train))
    print(grid.best_score_)
    print(grid.best_params_)

    model = grid.best_estimator_
    model.score(X_test, y_test)
    print(model.intercept_.shape) # X 
    print(model.coef_.shape) # matrice de poids

    b = model.intercept_
    w = model.coef_


    # pd.DataFrame(w).to_csv('/content/gdrive/My Drive/BE_IA_EMBARQUEE/coef.csv', index=False)
    # pd.DataFrame(b).to_csv('/content/gdrive/My Drive/BE_IA_EMBARQUEE/bias.csv', index=False)


    pd.DataFrame(w).to_csv('/data/coef.csv', index=False)
    pd.DataFrame(b).to_csv('/data/bias.csv', index=False)
    plot_confusion_matrix(model, X_test, y_test, normalize='true', )



def neural_network():
    """## **keras : NN**"""

    batch_size = 32

    train_dataset = (
        tf.data.Dataset
        .from_tensor_slices((X_train, y_train))
        .shuffle(799)
        .batch(batch_size)
    )

    test_dataset = (
        tf.data.Dataset
        .from_tensor_slices((X_test, y_test))
        .batch(batch_size)
    )

    valid_dataset = (
        tf.data.Dataset
        .from_tensor_slices((X_test, y_test))
        .batch(batch_size)
    )

    callbacks = [tf.keras.callbacks.EarlyStopping(patience=5, restore_best_weights=True)]

    model = tf.keras.models.Sequential()

    # Add the layers
    model.add(tf.keras.layers.Dense(22, activation="relu"))
    model.add(tf.keras.layers.Dense(16, activation="relu"))
    model.add(tf.keras.layers.Dense(10, activation="softmax"))

    model.compile(loss="sparse_categorical_crossentropy",
                optimizer=keras.optimizers.Adam(),
                metrics=['accuracy'])

    history = model.fit(train_dataset,
                        validation_data=valid_dataset,
                        epochs=100,
                        callbacks=callbacks)

    model_output = model.evaluate(test_dataset)

    """## **SKLearn : NN**

    # **Non optimisé**
    """

    SKL_NN = MLPClassifier(solver='lbfgs', alpha=1e-5, hidden_layer_sizes=(50), random_state=1)
    print(SKL_NN.fit(X_train, y_train), "\n")
    print(SKL_NN.score(X_test, y_test), "\n")
    y_pred = SKL_NN.predict(X_test)
    y_pred

    """# **Optimisé**"""

    param_grid = {
                        'hidden_layer_sizes': [15,50,(64,10), (64, 64), (50, 20)],
                        'activation': ['tanh', 'relu'],
                        'solver': ['sgd', 'adam'],
                        'alpha': [0.0001, 0.001, 0.01, 0.1, 0.5, 1],
                        'learning_rate': ['constant','adaptive'],
                        }
    model = MLPClassifier(max_iter=100)
    grid = GridSearchCV(model, param_grid, n_jobs=-1, cv=3)
    print(grid.fit(X_train, y_train))
    print(grid.best_score_)
    print(grid.best_params_)
    model = grid.best_estimator_
    model.score(X_test, y_test)


if __name__ == "__main__":
    SVM()
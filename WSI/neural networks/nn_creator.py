from sklearn.datasets import load_wine
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
import numpy as np
from ann_classifier import ANNClassifier
from network_layer import NetworkLayer
from functions import *


def split_data_into_sets(X_data: np.ndarray, Y_data: list, training_size: float, validate_size: float, test_size: float):
    """Randomly split samples from X_data into 3 data sets: trining, validating and testing one.

    Args:
        X_data (np.ndarray): List of samples, where each one a lisy numeric values which will determine the class of this sample
        Y_data (list): List of possible classes of samples from X_data
        training_size (float): Size of training data set. Value must be between 0 and 1.
        validate_size (float): Size of validating data set. Value must be between 0 and 1.
        test_size (float): Size of testing data set. Value must be between 0 and 1.
        Sum of training_sze, validate_size and test_size must be equal 1.

    Raises:
        Exception: When sum of training_sze, validate_size and test_size is not equal 1.

    Returns:
        Tuple: Tuple which consists 3 tuples where each one has list of samples and its posibble classes. Function returns train, validate and test data, in that order.
    """

    if not np.isclose(training_size + validate_size + test_size, 1):
        raise Exception("Error: sum of traiing_size, validate_size and test_size must be equal 1.")
    validate_size = validate_size / (1 - training_size)
    test_size = test_size / (1-training_size)

    X_train, X_testval, Y_train, Y_testval = train_test_split(X_data, Y_data, train_size=training_size, stratify=Y_data)

    X_test, X_validate, Y_test, Y_validate = train_test_split(X_testval, Y_testval, train_size=test_size, stratify=Y_testval)

    return (X_train, Y_train), (X_validate, Y_validate), (X_test, Y_test)


def create_neuron_network(normalized_X_train, Y_train, err_func, err_func_deriv, last_layer_info, hidden_layers_info):

    n_classes = len(np.unique(Y_train))
    n_inputs = len(normalized_X_train[0])

    netowrk = ANNClassifier(err_func, err_func_deriv, n_classes)

    if hidden_layers_info:
        previous_n_neurons = n_inputs
        for layer in hidden_layers_info:
            n_neurons, activ_func, activ_deriv = layer
            new_layer = NetworkLayer(previous_n_neurons, n_neurons, activ_func, activ_deriv)
            netowrk.add_layer(new_layer)
            previous_n_neurons = n_neurons
        lst_activ, lst_deriv = last_layer_info
        last_layer = NetworkLayer(previous_n_neurons, n_classes, lst_activ, lst_deriv)
        netowrk.add_layer(last_layer)
    else:
        lst_activ, lst_deriv = last_layer_info
        layer = NetworkLayer(n_inputs, n_classes, lst_activ, lst_deriv)

    return netowrk

def validate_classifier(classifier: ANNClassifier, X_validate, Y_validate):

    n_data = len(Y_validate)
    count = 0
    guesses = []
    for x_data, target in zip(X_validate, Y_validate):
        classification = classifier.classify_hard(x_data)
        if target == classification:
            count += 1
        guesses.append(classification)
    result = count/n_data
    return result, guesses

def normalize_data_to_minus_one_one(data):
    scaler = MinMaxScaler(feature_range=(-1, 1))
    scaler.fit(data)
    return scaler.transform(data)
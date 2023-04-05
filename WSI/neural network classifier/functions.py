import numpy as np


def mean_squared_error_function(classification, observed, class_count):
    return (classification - observed) ** 2 / class_count


def mean_squared_error_derivative_function(classification, observed, class_count):
    return 2 * (classification - observed) / class_count


def tanh_function(x):
    return np.tanh(x)


def tanh_derivative(x):
    return 1 - (np.tanh(x) ** 2)

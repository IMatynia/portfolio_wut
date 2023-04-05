from typing import Optional

import numpy as np


class NetworkLayer:
    """
    Neural connections between the previous layer and the current layer + nodes of the current layer
    """

    def __init__(self, n_of_inputs, n_of_outputs, activation_function, activation_function_derivative):
        self._n_inputs = n_of_inputs
        self._n_outputs = n_of_outputs
        self._activation = activation_function
        self._activation_derivative = activation_function_derivative
        self._weights: np.array = None
        """2D array of weights of all neurons,
        self._weights[a][b] -> weight between node a of the previous layer and node b of the current one"""
        self._biases: Optional[float] = None
        """1D array of biases of all neurons"""
        self._last_input = None

        self._weight_delta_batch = 0
        self._bias_delta_batch = 0
        self._batch_count = 0

    def forward_propagation(self, data: np.array) -> np.array:
        self._last_input = data
        return self._activation(np.dot(data, self._weights) + self._biases)

    def backward_propagation(self, error_on_output: np.array, learning_rate: float) -> np.array:
        """
        Optimises network weights and biases and calculates gradient
        :param error_on_output: dError/dInput on the next layer (1D array, next error for each node in this layer)
        :param learning_rate: stochastic gradient descent coefficient
        :return: next_error for the previous layer
        """

        temp = error_on_output * self._activation_derivative(np.dot(self._last_input, self._weights) + self._biases)
        # temp -> dE/d(wyjście tej warstwy)

        gradient_for_weights = np.matmul(self._last_input.T, temp)
        # gradient for each weight [a]->[b]

        self._weight_delta_batch -= learning_rate * gradient_for_weights
        self._bias_delta_batch -= learning_rate * temp
        self._batch_count += 1

        return np.dot(temp, self._weights.T)

    def apply_delta_batches(self):
        self._weights += self._weight_delta_batch * self._batch_count
        self._biases += self._bias_delta_batch * self._batch_count
        self._weight_delta_batch = 0
        self._bias_delta_batch = 0
        self._batch_count = 0

    def set_weights(self, new_weights: np.array):
        self._weights = new_weights

    def set_biases(self, new_biases: float):
        self._biases = new_biases

    def generate_random_weights_and_biases(self):
        self._bias_delta_batch = 0
        self._weight_delta_batch = 0
        self._batch_count = 0
        random_range = 1 / np.sqrt(self._n_inputs)
        self._weights = 2 * (np.random.random((self._n_inputs, self._n_outputs)) - 0.5) * random_range
        self._biases = 2 * (np.random.random((1, self._n_outputs)) - 0.5) * random_range

    def set_weights_and_biases_to_zero(self):
        self._bias_delta_batch = 0
        self._weight_delta_batch = 0
        self._batch_count = 0
        self._weights = np.zeros((self._n_inputs, self._n_outputs))
        self._biases = np.zeros((1, self._n_outputs))

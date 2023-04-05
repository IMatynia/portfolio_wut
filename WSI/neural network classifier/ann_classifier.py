import copy
import random
from typing import Callable, Tuple, List
import numpy as np
from network_layer import NetworkLayer


class ANNClassifier:
    def __init__(self, error_function: Callable, error_function_derivative: Callable, class_count: int):
        self._layers: list[NetworkLayer] = []
        self._err_func = error_function
        self._err_derivative = error_function_derivative
        self._class_count = class_count

    def add_layer(self, new_layer: NetworkLayer):
        self._layers.append(new_layer)

    def classify_raw(self, data: np.array) -> np.array:
        """
        Return the output array of probablities for each class
        """
        data = np.array(data)
        data = data.reshape((1, data.size))
        for net_layer in self._layers:
            data = net_layer.forward_propagation(data)
        return data

    def classify_hard(self, data: np.array) -> int:
        """
        Return estimated class number
        """
        classification = self.classify_raw(data)
        return classification.argmax()

    def train(self, training_data: tuple, epochs: int, learning_rate: float, mini_batch_count: int,
              validation_data: tuple = None) -> Tuple[List[List[float]], List[List[float]]]:
        """
        Trains the network with training data. If validation data is present, network will be valided after each epoch.
        Final weights and biases are set after validation, or if no validaation data is preset, the latest weights
        and biases.
        :param validation_data: optional, needed for validation
        :param mini_batch_count: how many mini batches to compute with
        :param training_data: data to train on
        :param epochs: number of epochs
        :param learning_rate: learning rate for stochastic gradient descent
        :return: 2D array of errors, where arr[x][y] means the error of estimating y-th data sample in x-th epoch
        """
        all_errors_on_training_data = []
        all_errors_on_validation_data = []
        best_validation_error = 100000.0
        best_network_state = None

        samples, classes = training_data
        for i in range(epochs):
            epoch_train_errors = []
            epoch_validation_errors = []
            all_samples_with_classes = list(zip(samples, classes))
            random.shuffle(all_samples_with_classes)
            for batch in np.array_split(all_samples_with_classes, mini_batch_count):
                for sample, observed in batch:
                    classification = self.classify_raw(sample)
                    observed_array = -np.ones(self._class_count)
                    observed_array[observed] = 1.0
                    observed_array = observed_array.reshape((1, observed_array.size))

                    error = self._err_func(classification, observed_array, self._class_count)
                    epoch_train_errors.append(error)
                    error_derivative = self._err_derivative(classification, observed_array, self._class_count)

                    for net_layer in reversed(self._layers):
                        error_derivative = net_layer.backward_propagation(error_derivative, learning_rate)
                # apply mini batch deltas
                for net_layer in self._layers:
                    net_layer.apply_delta_batches()

            # validation errors after this epoch of training
            total_validation_error = 0

            if validation_data is not None:
                samples, classes = validation_data
                for sample, observed in zip(samples, classes):
                    classification = self.classify_raw(sample)
                    observed_array = -np.ones(self._class_count)
                    observed_array[observed] = 1.0
                    observed_array = observed_array.reshape((1, observed_array.size))
                    error = self._err_func(classification, observed_array, self._class_count)
                    epoch_validation_errors.append(error)
                    total_validation_error += np.average(error)
                if total_validation_error < best_validation_error:
                    best_validation_error = total_validation_error
                    best_network_state = copy.deepcopy(self._layers)
            all_errors_on_training_data.append(epoch_train_errors)
            all_errors_on_validation_data.append(epoch_validation_errors)

        if validation_data is not None:
            # set network state after validation
            self._layers = best_network_state
        return all_errors_on_training_data, [all_errors_on_validation_data[0]] + all_errors_on_validation_data[:-1]

    def randomize_weights_and_biases(self):
        for i in range(len(self._layers) - 1):
            self._layers[i].generate_random_weights_and_biases()

        # set the last layer to have zeros as parameters
        self._layers[-1].set_weights_and_biases_to_zero()

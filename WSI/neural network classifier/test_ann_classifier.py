from ann_classifier import ANNClassifier
from network_layer import NetworkLayer
from nn_creator import create_neuron_network
from functions import *

def test_ann_classifier():

    correct = 0
    expected = 100
    xor_X = [
        [0, 0],
        [0, 1],
        [1, 0],
        [1, 1]]
    xor_Y = [0, 1, 1, 0]

    for  _ in range(expected):


        classifier = ANNClassifier(mean_squared_error_function, mean_squared_error_derivative_function, 2)

        input_netowrk_layer = NetworkLayer(2, 3, tanh_function, tanh_derivative)
        hidden_layer = NetworkLayer(3, 2, tanh_function, tanh_derivative)

        classifier.add_layer(input_netowrk_layer)
        classifier.add_layer(hidden_layer)

        classifier.randomize_weights_and_biases()

        classifier.train((xor_X, xor_Y), 1000, 0.02, 2)

        try:

            assert (classifier.classify_hard([0, 0]) == 0)
            assert (classifier.classify_hard([0, 1]) == 1)
            assert (classifier.classify_hard([1, 1]) == 0)
            assert (classifier.classify_hard([1, 0]) == 1)

            correct += 1

        except Exception:
            continue


    assert(correct >= 0.90*expected)

def test_ann_classificator_creator():

    correct = 0
    expected = 100
    xor_X = [
        [0, 0],
        [0, 1],
        [1, 0],
        [1, 1]]
    xor_Y = [0, 1, 1, 0]


    hidden_layers_data = [[3, tanh_function, tanh_derivative]]
    last_layer_data =   [tanh_function, tanh_derivative]

    network = create_neuron_network(xor_X, xor_Y, mean_squared_error_function, mean_squared_error_derivative_function, last_layer_data, hidden_layers_data)

    assert(len(network._layers) == 2)
    assert(network._layers[0]._activation == tanh_function)
    assert(network._layers[0]._activation_derivative == tanh_derivative)
    assert(network._layers[1]._activation == tanh_function)
    assert(network._layers[1]._activation_derivative == tanh_derivative)

    assert(network._layers[0]._n_inputs == 2)
    assert(network._layers[0]._n_outputs == 3)
    assert(network._layers[1]._n_inputs == 3)
    assert(network._layers[1]._n_outputs == 2)

    correct = 0
    expected = 100

    for  _ in range(expected):

        network.randomize_weights_and_biases()
        network.train((xor_X, xor_Y), 1000, 0.1, 2, (xor_X, xor_Y))
        try:
            assert (network.classify_hard([0, 0]) == 0)
            assert (network.classify_hard([0, 1]) == 1)
            assert (network.classify_hard([1, 1]) == 0)
            assert (network.classify_hard([1, 0]) == 1)
            correct += 1
        except Exception:
            continue

    assert(correct >= 0.75*expected)

    more_hidden_layers = [[3,tanh_function, tanh_derivative],[4, tanh_function, tanh_derivative]]

    bigger_network = create_neuron_network(xor_X, xor_Y, mean_squared_error_function, mean_squared_error_derivative_function, last_layer_data, more_hidden_layers)

    assert(len(bigger_network._layers) == 3)
    assert(bigger_network._layers[0]._activation == tanh_function)
    assert(bigger_network._layers[0]._activation_derivative == tanh_derivative)
    assert(bigger_network._layers[1]._activation == tanh_function)
    assert(bigger_network._layers[1]._activation_derivative == tanh_derivative)
    assert(bigger_network._layers[2]._activation == tanh_function)
    assert(bigger_network._layers[2]._activation_derivative == tanh_derivative)

    assert(bigger_network._layers[0]._n_inputs == 2)
    assert(bigger_network._layers[0]._n_outputs == 3)
    assert(bigger_network._layers[1]._n_inputs == 3)
    assert(bigger_network._layers[1]._n_outputs == 4)
    assert(bigger_network._layers[2]._n_inputs == 4)
    assert(bigger_network._layers[2]._n_outputs == 2)

    correct_bigger = 0
    expected_bigger = 100

    for  _ in range(expected_bigger):

        bigger_network.randomize_weights_and_biases()
        bigger_network.train((xor_X, xor_Y), 1000, 0.01, 2, (xor_X, xor_Y))
        try:
            assert (bigger_network.classify_hard([0, 0]) == 0)
            assert (bigger_network.classify_hard([0, 1]) == 1)
            assert (bigger_network.classify_hard([1, 1]) == 0)
            assert (bigger_network.classify_hard([1, 0]) == 1)
            correct_bigger += 1

        except Exception:
            continue

    assert(correct_bigger >= 0.75*expected_bigger)





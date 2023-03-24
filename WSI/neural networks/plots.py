import matplotlib.pyplot as plt
from matplotlib import patches as mpatches
import numpy as np
from functions import *
from nn_creator import split_data_into_sets, create_neuron_network, validate_classifier, normalize_data_to_minus_one_one
from sklearn.datasets import load_wine

import warnings
warnings.filterwarnings('ignore')


def plot_classification_results(hiperparams_set, hiperparam_name, hiperparams_desc, n_runs, datasets_size=[0.8,0.1,0.1]):

    train_s, vali_s, test_s = datasets_size
    all_results = [[] for _ in hiperparams_set]

    raw_data = load_wine()
    X_raw_data = normalize_data_to_minus_one_one(raw_data.data)
    Y_raw_data = raw_data.target

    for _ in range(n_runs):
        XY_train, XY_validate, XY_test = split_data_into_sets(X_raw_data, Y_raw_data, train_s, vali_s, test_s)
        X_train, Y_train = XY_train
        X_validate, Y_validate = XY_validate
        X_test, Y_test = XY_test
        for index, hiperparam in enumerate(hiperparams_set):
            err_func, err_func_deriv, last_lay_info, hidden_lay_info, n_epoch, learn_rate, n_batch = hiperparam
            network = create_neuron_network(X_train, Y_train, err_func, err_func_deriv, last_lay_info, hidden_lay_info)
            network.randomize_weights_and_biases()
            network.train((X_train, Y_train), n_epoch, learn_rate, n_batch, (X_validate, Y_validate))
            result, _ = validate_classifier(network, X_test, Y_test)
            all_results[index].append(result)


    all_results = np.average(all_results, axis=1)
    fig, ax = plt.subplots()
    bars = ax.bar(hiperparams_desc, all_results)
    ax.bar_label(bars)
    plt.title(f"Wykress zależności średniej dokładności sieci neuronowej w zależności od: {hiperparam_name}. Średnia dla ilości prób: {n_runs}")
    ax.set_xlabel(hiperparam_name)
    ax.set_ylabel('Dokładność')
    plt.show()


def plot_classification_results_history(hyperparams, hyperparams_labels, n_runs, datasets_size=[0.8,0.1,0.1]):

    def make_patch_spines_invisible(ax):
        ax.set_frame_on(True)
        ax.patch.set_visible(False)
        for sp in ax.spines.values():
            sp.set_visible(False)

    legend_patches = []
    hyperparam_error_train = [[] for _ in range(len(hyperparams))]
    hyperparam_error_validate = [[] for _ in range(len(hyperparams))]
    train_s, vali_s, test_s = datasets_size
    single_ax_offset = 0.07
    fig, host_ax = plt.subplots(figsize = (10, 5))
    fig.subplots_adjust(top=1/(1-single_ax_offset+single_ax_offset*len(hyperparams)))

    raw_data = load_wine()
    X_raw_data = normalize_data_to_minus_one_one(raw_data.data)
    Y_raw_data = raw_data.target

    for i in range(n_runs):
        XY_train, XY_validate, _ = split_data_into_sets(X_raw_data, Y_raw_data, train_s, vali_s, test_s)
        X_train, Y_train = XY_train
        X_validate, Y_validate = XY_validate
        for index, hyperparam in enumerate(hyperparams):
            err_func, err_func_deriv, fl_lay_info, hidden_lay_info, n_epoch, learn_rate, n_batches = hyperparam
            network = create_neuron_network(X_train, Y_train, err_func, err_func_deriv, fl_lay_info, hidden_lay_info)
            network.randomize_weights_and_biases()
            train_history, validate_history = network.train((X_train, Y_train), n_epoch, learn_rate, n_batches, (X_validate, Y_validate))
            flattened_history = []
            flattened_history_validate = []
            for line in train_history:
                arry = np.array(line)
                flat = arry.flatten()
                flattened_history.append(np.average(flat))
            for line in validate_history:
                arry = np.array(line)
                flat = arry.flatten()
                flattened_history_validate.append(np.average(flat))
            hyperparam_error_train[index].append(flattened_history)
            hyperparam_error_validate[index].append(flattened_history_validate)

    plot_data = []
    for data in hyperparam_error_train:
        data = np.array(data)
        plot_data.append(np.average(data, axis=0))

    plot_data_stddev = []
    for data in hyperparam_error_validate:
        data = np.array(data)
        plot_data_stddev.append(np.average(data, axis=0))

    legend_patches = [mpatches.Patch(color=f"C{index}") for index in range(len(hyperparams))]


    host_ax.plot(np.arange(len(plot_data[0])), plot_data[0])
    host_ax.plot(np.arange(len(plot_data_stddev[0])), plot_data_stddev[0], c='C0', ls='--')
    host_ax.tick_params(axis='x', colors='C0')
    current_offset = 1-single_ax_offset
    for index, data in enumerate(plot_data[1:]):
        new_ax = host_ax.twiny()
        make_patch_spines_invisible(new_ax)
        new_ax.plot(np.arange(len(data)), data, color=f'C{1+index}')
        new_ax.plot(np.arange(len(data)), plot_data_stddev[index+1], color=f'C{1+index}', ls='--')
        current_offset += single_ax_offset
        new_ax.spines.top.set_position(('axes', current_offset))
        new_ax.spines.top.set_visible(True)
        new_ax.tick_params(axis='x', colors=f'C{1+index}')

    host_ax.set(xlabel="Epoka", ylabel='Średnia wartość błędu')
    plt.subplots_adjust(right=0.85)
    plt.suptitle(f"Wykres średniej wartości błędu sieci neuronowej w zależnosci od\nzastosowanych hiperparamatrów i generacji sieci neuronowej. średnia dla ilości prób: {n_runs}", fontsize=10)
    plt.figlegend(handles=legend_patches, labels=hyperparams_labels, loc="center right")
    fig.subplots_adjust(right=0.75)
    fig.subplots_adjust(top=0.80)
    plt.show()


if __name__ == '__main__':
    lli = [tanh_function, tanh_derivative]
    hli = [[3, tanh_function, tanh_derivative]]

    lli2 = [tanh_function, tanh_derivative]
    hli2 = [[6, tanh_function, tanh_derivative]]
    hyperparams_set = [[mean_squared_error_function, mean_squared_error_derivative_function, lli, hli, 1000, 0.01, 150]
    ,[mean_squared_error_function, mean_squared_error_derivative_function, lli2, hli2, 1000, 0.01, 150]]
    # plot_classification_results_history(hyperparams_set, ['Te','st'], 10)

    plot_classification_results(hyperparams_set, 'test', ['Te','st'], 10)

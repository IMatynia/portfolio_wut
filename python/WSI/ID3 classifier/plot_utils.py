import numpy as np
from matplotlib import pyplot as plt
from id3classifier import id3, grade_decision_tree
from data_handling import ATTRIBUTES, get_breast_cancer_samples
from copy import copy
import multiprocess
import functools


def compute_sample(i: int, depth: int, attributes: list, classifier: str) -> float:
    training, validation, testing = get_breast_cancer_samples(0.75, 0.25, 0.0)
    decision_tree = id3(training, copy(attributes), classifier, depth)
    return grade_decision_tree(decision_tree, validation, classifier)


def plot_tree_depth_over_accuracy(samples: int, depths: list):
    classifier = "irradiat"
    attributes = copy(ATTRIBUTES)

    results = {}

    for depth in depths:
        print(f"Calculating samples for depth {depth}")
        pool_size = multiprocess.cpu_count() * 2
        pool = multiprocess.Pool(processes=pool_size,
                                 maxtasksperchild=2,
                                 )
        pool_outputs = pool.map(functools.partial(
            compute_sample, depth=depth, attributes=attributes, classifier=classifier), range(samples))
        pool.close()
        pool.join()
        results[depth] = copy(pool_outputs)

    means = [np.mean(results[depth]) for depth in results]
    std_deviation = [np.std(results[depth]) for depth in results]
    labels = list(map(str, results.keys()))

    plt.errorbar(labels, means, std_deviation, linestyle='None', fmt="o")
    plt.title(f"Mean accuracy of ID3 classifier with given tree depth over {samples} samples")
    plt.xlabel("Max tree depth")
    plt.ylabel("Mean accuracy")
    plt.show()

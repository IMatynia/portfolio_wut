import math

import numpy as np
import pandas as pd
from copy import copy
import random


class Node:
    pass


class DecisionNode(Node):
    def __init__(self, attribute):
        self._childern = {}
        self._attribute = attribute

    def add_child(self, value, child_node):
        self._childern[value] = child_node

    def get_decision(self, value):
        if value in self._childern:
            return self._childern[value]
        else:
            return random.choice(list(self._childern.values()))

    def get_attribute(self):
        return self._attribute

    def __str__(self):
        return f"{self._attribute} node, has {len(self._childern)} childern"


class ClassificationNode(Node):
    def __init__(self, classification):
        self._class = classification

    def get_classification(self):
        return self._class

    def __str__(self):
        return f"Terminal node, classifing as {self._class}"


def classify_unit(data: dict, tree_root: DecisionNode) -> ClassificationNode:
    next_node = tree_root
    timeout = 100
    while not isinstance(next_node, ClassificationNode) and timeout > 0:
        value = data[next_node.get_attribute()]
        next_node = next_node.get_decision(value)
        timeout -= 1

    return next_node


def id3(training_data: pd.DataFrame, attributes: list, classifier: str, depth: int) -> Node:
    attributes = copy(attributes)
    grouped_by_class = training_data.groupby(classifier)

    # All remaining data has the same classifier
    if len(grouped_by_class.size()) == 1:
        current_class = grouped_by_class[classifier].unique()[0]
        new_terminal_node = ClassificationNode(current_class)
        return new_terminal_node

    # No attributes remain, return the most likely classification
    if len(attributes) == 0 or depth == 0:
        counts = grouped_by_class \
            .size() \
            .reset_index(name="count") \
            .sort_values(['count'], ascending=False) \
            .head(1)
        current_class = counts[classifier].unique()[0]
        new_terminal_node = ClassificationNode(current_class)
        return new_terminal_node

    # Choose the attribute with the greatest entropy
    best_attribute = None
    best_infgain = -math.inf
    for attribute in attributes:
        inf_gain = information_gain(attribute, training_data, classifier)
        if inf_gain > best_infgain:
            best_infgain = inf_gain
            best_attribute = attribute

    # Recurrently build tree
    grouped_by_best_attribute = training_data.groupby(best_attribute)
    unique_values = training_data[best_attribute].unique()
    new_decision_node = DecisionNode(best_attribute)
    attributes.remove(best_attribute)
    for value in unique_values:
        new_dataset = grouped_by_best_attribute.get_group(value).drop(columns=[best_attribute])
        subtree = id3(new_dataset, attributes, classifier, depth - 1)
        new_decision_node.add_child(value, subtree)

    return new_decision_node


def best_depth_id3(validation_data: pd.DataFrame, training_data: pd.DataFrame, attributes: list, classifier: str, max_depth: int = 10000000) -> Node:
    best_score = 0
    best_tree = None
    for depth in range(min(len(attributes), max_depth)):
        dec_tree = id3(training_data, attributes, classifier, depth)
        score = grade_decision_tree(dec_tree, validation_data, classifier)
        if score > best_score:
            best_tree = dec_tree
            best_score = score

    return best_tree


def entropy_of_dataset(data: pd.DataFrame, classifier: str) -> float:
    counts = data.groupby(classifier).size().reset_index(name="count")
    return sum(-math.log(row["count"]) * row["count"] for i, row in counts.iterrows())


def entropy_of_subsets(dividing_attribute: str, data: pd.DataFrame, classifier: str) -> float:
    unique_values = data[dividing_attribute].unique()
    grouped = data.groupby(dividing_attribute)
    total_entropy = 0
    for value in unique_values:
        subset = grouped.get_group(value)
        total_entropy += len(subset) / len(data) * entropy_of_dataset(subset, classifier)
    return total_entropy


def information_gain(dividing_attribute: str, data: pd.DataFrame, classifier: str):
    return entropy_of_dataset(data, classifier) - entropy_of_subsets(dividing_attribute, data, classifier)


def grade_decision_tree(tree_root: Node, data_set: pd.DataFrame, classifier: str):
    correct_classifications = 0
    for i, row in data_set.iterrows():
        classification = classify_unit(row.to_dict(), tree_root)
        if classification.get_classification() == row[classifier]:
            correct_classifications += 1

    return correct_classifications/len(data_set)



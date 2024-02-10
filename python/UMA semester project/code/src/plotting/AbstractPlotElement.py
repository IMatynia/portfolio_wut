"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""


from matplotlib import pyplot as plt
from src.simple_experiment import Experiment


class AbstractPlotElement:
    def __init__(self, axis: plt.Axes, experiments: list[Experiment], title: str):
        self.axis = axis
        self.experiments = experiments
        self.title = title

    def plot(self):
        raise NotImplementedError("Plot method not implemented")

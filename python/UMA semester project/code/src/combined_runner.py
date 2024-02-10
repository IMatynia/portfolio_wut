"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

from typing import List
from src.simple_experiment import RunnableExperiment
from src.plotting.SimplePlotElements import *
from matplotlib import pyplot as plt
from src.qlearning.lineardiscretisation import *
from src.functions.function import Function


class CombinedRunner:
    def __init__(self):
        pass

    def set_up_experiments(self):
        experiments = [
            RunnableExperiment(
                "test",
                {
                    "gamma": 0.9,
                    "epsilon": lambda x: 0.3,
                    "beta": 0.5,
                    "episodes": 1,
                    "steps": 500,
                    "state_space": StateSpaceLinearDescretisation(
                        5, 10, 5, 10
                    ),
                    "action_space": ActionSpaceLiearDescretisation(
                        [0.6, 0.75, 0.9], [0.5, 1.0, 1.5, 2.0]
                    ),
                    "function": Function(
                        13, 10, -1, 1
                    ),
                    "iterations_per_step": 10
                }
            )
        ]
        return experiments

    def plot_experiments(self, experiments: List[RunnableExperiment]):
        rows = 1
        cols = 1
        fig, axes = plt.subplots(rows, cols)

        plot_objects = [
            RewardsOverEpisodesAndSteps(axes, experiments, "Rewards over episodes")
        ]

        for plot_object in plot_objects:
            plot_object.plot()

        plt.show()

    def main_loop(self):
        experiments = self.set_up_experiments()
        print("Experiment set up")

        for experiment in experiments:
            experiment.run()

        print("Experiments finished")
        self.plot_experiments(experiments)
        print("Plotting finished")

"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

import multiprocessing
import os
import pickle

from matplotlib import pyplot as plt

from src.functions.function import Function
from src.plotting.SimplePlotElements import RewardsOverEpisodesAndStepsAveragedOverExperiments
from src.qlearning.lineardiscretisation import ActionSpaceLiearDescretisation, StateSpaceLinearDescretisation
from src.simple_experiment import *

LOW = 0
HIGH = 1


class Trainer:
    def __init__(self):
        self.gamma = 0.98

        self.beta_high = 0.40
        self.beta_low = 0.15

        self.epsilon_high = 0.7
        self.epsilon_low = 0.15

        self.dimentions = [10, 30, 50]
        self.functions = [1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29]
        self.episodes = 10
        self.snapshot_interval = 9
        self.iterations_per_step = 5
        self.snapshot_folder = './rl_model_snapshots'
        self.action_space = ActionSpaceLiearDescretisation(
            [0.6, 0.75, 0.9], [0.6, 0.75, 1.0, 1.2, 1.8]
        )
        self.steps_per_episode = 150
        self.learning_repetitions = 30

    def get_beta(self, repeat: int):
        return self.beta_high - repeat * (self.beta_high - self.beta_low) / self.learning_repetitions

    def get_epsilon(self, repeat: int):
        return self.epsilon_high - repeat * (self.epsilon_high - self.epsilon_low) / self.learning_repetitions

    def init(self):
        if not os.path.exists(self.snapshot_folder):
            os.mkdir(self.snapshot_folder)

    def runTraining(self):
        num_processes = multiprocessing.cpu_count()
        res = []
        with multiprocessing.Pool(processes=num_processes) as pool:
            res = pool.map(self.trainDimention, self.dimentions)

    def trainDimention(self, dimention: int):
        # one agent per dimentionality
        state_space = StateSpaceLinearDescretisation(HIGH, 10, 5, dimention)
        agent = Qlearning(self.gamma, self.epsilon_high, self.beta_high, state_space, self.action_space,
                          self.snapshot_interval)
        for learning_repetition in range(self.learning_repetitions):
            # epsilon and beta values are decreasing with number of repetitons
            agent.epsilon = self.get_epsilon(learning_repetition)
            agent.beta = self.get_beta(learning_repetition)
            for function_num in self.functions:
                print(f"Dim: {dimention} Rep: {learning_repetition} Func: {function_num}")
                agent.flush_plotting_data()
                function = Function(function_num, dimention, LOW, HIGH)
                env = QLearningEnvironment(state_space, self.action_space, function, self.iterations_per_step)
                friendly_name = f"{learning_repetition};{dimention};{function_num}"
                agent.learn(self.episodes, self.steps_per_episode, env, self.snapshot_folder,
                            friendly_name)
                with open(f"{self.snapshot_folder}/{friendly_name}_stats.bin", "wb") as file:
                    pickle.dump(agent.plotting_data, file)

    def run_policy(self):
        plot_func_number = 5
        reps = 25
        iterations = 100

        # OLD
        repetition = 15
        dimnetion = 30
        func_number = 9
        episode = 2

        cache_file_name = f"{self.snapshot_folder}/{repetition};{dimnetion};{func_number};{episode}.npy"
        state_space = StateSpaceLinearDescretisation(HIGH, 10, 5, dimnetion)
        function = Function(plot_func_number, dimnetion, LOW, HIGH)
        env = QLearningEnvironment(state_space, self.action_space, function, self.iterations_per_step)

        experimentsOld = []
        for i in range(reps):
            print("RepOld", i)
            agent = Qlearning(0, 0, 0, state_space, self.action_space, self.snapshot_interval)
            agent.read_policy(cache_file_name)
            agent.learn(
                1, iterations, env
            )
            experiment = Experiment("test", {}, agent.plotting_data)
            experimentsOld.append(experiment)

        # NEW
        repetition = 29
        dimnetion = 30
        func_number = 9
        episode = 2

        cache_file_name = f"{self.snapshot_folder}/{repetition};{dimnetion};{func_number};{episode}.npy"
        state_space = StateSpaceLinearDescretisation(HIGH, 10, 5, dimnetion)
        function = Function(plot_func_number, dimnetion, LOW, HIGH)
        env = QLearningEnvironment(state_space, self.action_space, function, self.iterations_per_step)

        experimentsNew = []
        for i in range(reps):
            print("RepNew", i)
            agent = Qlearning(0, 0, 0, state_space, self.action_space, self.snapshot_interval)
            agent.read_policy(cache_file_name)
            agent.learn(
                1, iterations, env
            )
            experiment = Experiment("test", {}, agent.plotting_data)
            experimentsNew.append(experiment)

        fig, ax = plt.subplots(1, 1)

        plot_obj = RewardsOverEpisodesAndStepsAveragedOverExperiments(ax, experimentsNew, "NEW")
        plot_obj.plot()

        plot_obj = RewardsOverEpisodesAndStepsAveragedOverExperiments(ax, experimentsOld, "OLD")
        plot_obj.plot()

        ax.set_title("Old policy vs new policy comparison")

        fig.show()


if __name__ == "__main__":
    trainer = Trainer()
    trainer.init()
    trainer.run_policy()
    # trainer.runTraining()

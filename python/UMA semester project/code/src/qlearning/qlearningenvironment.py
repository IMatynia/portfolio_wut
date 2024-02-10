"""
Projekt UMA 23Z
Autorzy:
Filip Stefański
Igor Matynia
"""

from src.differential_evolution.differential_evolution import DifferentialEvolution
from src.functions.function import Function
from src.qlearning.lineardiscretisation import StateSpaceLinearDescretisation, ActionSpaceLiearDescretisation


class QLearningEnvironment:
    def __init__(self, state_space: StateSpaceLinearDescretisation, action_space: ActionSpaceLiearDescretisation,
                 function: Function, iterations_per_step: int):
        self.last_avg = 0
        self.state_space = state_space
        self.action_space = action_space
        self.function = function
        self.iterations_per_step = iterations_per_step
        self.differential_evolution = DifferentialEvolution(function, self.iterations_per_step)

    def get_initial_state(self) -> tuple:
        """
        Provides initial state of the population
        :return:
        """
        average_distance = self.differential_evolution.average_distance(self.differential_evolution.P)
        success_percent = 0

        return average_distance, success_percent

    def do_action(self, action_representation: tuple):
        """
        Sets the parameters of the evolution and runs with them
        :param action_representation: new evolution parameters
        :return: returns reward and new state tuple (representation)
        """
        F_value, selection_type, crossover_type, CR_value = action_representation[0], 0, \
            0, action_representation[1]
        reward, next_state_actual = self.differential_evolution.do_evolution(F_value=F_value,
                                                                             selection_type=selection_type,
                                                                             crossover_type=crossover_type,
                                                                             CR_value=CR_value)

        return reward, next_state_actual

    def reset_environment(self):
        self.differential_evolution = DifferentialEvolution(self.function, self.iterations_per_step)
        self.last_avg = 0

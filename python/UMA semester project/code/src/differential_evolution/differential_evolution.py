"""
Projekt UMA 23Z
Autorzy:
Filip Stefański
Igor Matynia
"""

import random

import numpy as np

from src.functions.function import Function


class DifferentialEvolution:
    def __init__(self, function: Function, iterations: int):
        self.C = 10000
        self.function = function
        self._population_size = 20
        self.P = self.generate_uniform_population()
        self.old_func_values = np.full(self._population_size, 0xfffffffffffffffffffff, np.float32)
        self._number_of_iterations = iterations

    def do_evolution(self, F_value, selection_type, crossover_type, CR_value):
        """
        Use this function to do one iteration of differential evolution. Function does self._number_of_iterations iterations.
        Each iteration runs  self._population_size times.
        :param F_value: F for iteration
        :param selection_type: type of selection/modulation: 0 -> random, 1 -> best, 2 -> local-to-best
        :param crossover_type: type of crossover: 0 -> bin, 1 -> exp
        :param CR_value: CR for iteration
        :return: reward, tuple representing next_state
        """
        success_counter = 0
        step_counter = 0
        # iterate evolution
        for i in range(self._number_of_iterations):
            # generating the new population
            for z in range(len(self.P)):
                x_w = self.select(self.P, selection_type)

                # Get x_1
                x_1 = np.random.randint(self._population_size - 1)
                if x_1 >= x_w:
                    x_1 += 1

                # Get x_2
                x_2 = np.random.randint(self._population_size - 2)
                if x_2 >= min(x_w, x_1):
                    x_2 += 1
                if x_2 >= max(x_w, x_1):
                    x_2 += 1

                u = self.mutate(x_w, x_1, x_2, z, selection_type, F_value, self.P)
                x_k = self.crossover(u, self.P[z], crossover_type, CR_value)

                func_value_for_mutant = self.function(x_k)
                if func_value_for_mutant <= self.old_func_values[z]:
                    self.P[z] = x_k
                    self.old_func_values[z] = func_value_for_mutant
                    success_counter += 1
                step_counter += 1

        # REWARD CALCULATION
        reward_base = (0.9 * np.min(self.old_func_values) + 0.1 * np.average(self.old_func_values)) / self.C
        reward = self.C * (1 - np.tanh(reward_base))
        return reward, (self.average_distance(self.P), success_counter / step_counter)

    def average_distance(self, P):
        distance = 0
        count = 0
        for i in range(0, self._population_size):
            for j in range(i, self._population_size):
                distance += np.sqrt(np.sum((P[i] - P[j]) ** 2))
                count += 1
        return distance / count

    def max_distance(self, P):
        distance_max = 0
        for i in range(0, self._population_size):
            for j in range(i, self._population_size):
                distance_max = max(distance_max, np.sqrt(np.sum((P[i] - P[j]) ** 2)))
        return distance_max

    def generate_uniform_population(self):
        return np.random.uniform(low=self.function.low, high=self.function.high,
                                 size=(self._population_size, self.function.n_dimensions))

    def select(self, P, selection_type):
        """
        Function that selects index from P based on the selection_type.
        :param P: population
        :param selection_type: type of selection used in selection process
        :param P: population
        :return: selected index
        """
        # 0 = rand
        if selection_type == 0:
            return random.randint(0, self._population_size - 1)
        # 1 = best
        # 2 = local-to-best
        elif selection_type == 1 or selection_type == 2:
            minimum_val = np.inf
            minimum_index = -1
            for i, x in enumerate(P):
                val = self.function(x)
                if val < minimum_val:
                    minimum_val = val
                    minimum_index = i
            return minimum_index
        else:
            print("Wrong selection type!")

    def mutate(self, x_w, x_1, x_2, x_i, selection_type, F_value, P):
        """
        Function that does mutation in differential evolution.
        :param x_w: index of P
        :param x_1: index of P
        :param x_2: index of P
        :param x_i: index of P
        :param selection_type: type of selection used in mutation process
        :param F_value: value of F
        :param P: population
        :return: created point, point is a table variable []
        """
        # 0 = random
        # 1 = best
        if selection_type == 0 or selection_type == 1:
            return [x + F_value * (y - z) for x, y, z in zip(P[x_w], P[x_1], P[x_2])]
        # 2 = local-to-best
        elif selection_type == 2:
            return [x + F_value * (y - x) + F_value * (z - v) for x, y, z, v in zip(P[x_i], P[x_w], P[x_1], P[x_2])]
        else:
            raise Exception("Wrong selection type!")

    def crossover(self, u, x, crossover_type, CR):
        """
        Function that does crossover in differential evolution.
        :param u: one point of P, type table []
        :param x:  one point of P, type table []
        :param crossover_type: type of crossover used in crossover process
        :param CR: value of CR
        :return: point made after crossover, point is a table variable []
        """
        n = self.function.n_dimensions
        x_z = [None] * n

        # 0 = bin
        if crossover_type == 0:
            for i in range(0, n):
                a = random.random()
                if a < CR:
                    x_z[i] = x[i]
                else:
                    x_z[i] = u[i]
            return x_z

        # 1 = exp
        elif crossover_type == 1:
            i = 0
            j = random.randint(0, n - 1)
            while i <= n:
                a = random.random()
                if a < CR:
                    x_z[j] = x[j]
                else:
                    break
                i += 1
                j = (j + 1) % n
            while i <= n:
                x_z[j] = u[j]
                i += 1
                j = (j + 1) % n
            return x_z

        else:
            raise Exception("Wrong crossover type!")

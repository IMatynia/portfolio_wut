"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

import numpy as np


class StateSpaceLinearDescretisation:
    def __init__(self, distance_max_normalized, distance_quanta, success_percent_quanta, dimentionality: int):
        self._distance_max = distance_max_normalized * (dimentionality ** 0.5)
        self._distance_quanta = distance_quanta
        self._success_percent_quanta = success_percent_quanta

    def get_state_representation(self, actual_distance: float, actual_success_percent: float):
        distance_index = max(0, min(self._distance_quanta - 1,
                                    np.floor((actual_distance / self._distance_max) * self._distance_quanta).astype(
                                        int)))
        successes_index = max(0, min(self._success_percent_quanta - 1,
                                     np.floor(actual_success_percent * self._success_percent_quanta).astype(int)))
        return distance_index * self._success_percent_quanta + successes_index

    def get_shape(self):
        return self._distance_quanta * self._success_percent_quanta

    def get_random(self):
        return np.random.randint(self.get_shape())


class ActionSpaceLiearDescretisation:
    def __init__(self, cr_values: list, F_values: list):
        self._cr_values = cr_values
        self._F_values = F_values

    def get_action_from_representation(self, representation: int):
        F_index = representation % len(self._F_values)
        representation //= len(self._F_values)
        cr = representation

        return self._F_values[F_index], self._cr_values[cr]

    def get_shape(self):
        return len(self._cr_values) * len(self._F_values)

    def get_random(self):
        return np.random.randint(self.get_shape())

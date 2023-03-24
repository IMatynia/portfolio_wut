import numpy as np


class Population:
    @staticmethod
    def generate(n_of_samples: int, limit: float):
        specimen = (np.random.rand(n_of_samples, 2)*2-1)*limit
        return Population(specimen)

    def __init__(self, specimen: np.array = None, scores: np.array = None) -> None:
        self.specimen = specimen if specimen is not None else np.array([])
        self._scores = scores if scores is not None else np.array([])

    def get_scores(self):
        return self._scores

    def set_scores(self, scores: np.array):
        self._scores = scores

    def find_best(self):
        max_it = -1
        max_val = -1
        for it, el in enumerate(self._scores):
            if el > max_val:
                max_val = el
                max_it = it
        return max_val, self.specimen[max_it]

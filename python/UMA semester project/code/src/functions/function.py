"""
Projekt UMA 23Z
Autorzy:
Filip Stefański
"""


import cec2017.functions as functions


class Function:
    def __init__(self, function_number, n_dimensions, low, high):
        self._function = functions.all_functions[function_number - 1]
        self.n_dimensions = n_dimensions
        self.low = low
        self.high = high
        self.n_calls = 0

    def __call__(self, *args, **kwargs):
        self.n_calls += 1
        return self._function(*args, **kwargs)

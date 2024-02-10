from game_board import BoardState
from constants import P_MAX, P_MIN
import time
from multiprocess_experiment import compute_experiment
from constants import *


def main():
    res = compute_experiment(
        samples=100,
        n=3,
        player=P_MIN,
        p_min_depth=9,
        p_max_depth=9
    )
    print(res)


if __name__ == "__main__":
    main()

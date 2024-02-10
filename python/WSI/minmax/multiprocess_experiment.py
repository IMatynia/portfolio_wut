import multiprocess
from minmax import minmax_competition
from game_board import BoardState
import functools


def experiment_sample(i, n, player, p_min_depth, p_max_depth):
    b = BoardState(n)
    res = minmax_competition(b, player, p_min_depth, p_max_depth)
    return res


def compute_experiment(samples: int, n: int, player: int, p_min_depth: int, p_max_depth: int):
    pool_size = multiprocess.cpu_count() * 2
    pool = multiprocess.Pool(processes=pool_size,
                             maxtasksperchild=2,
                             )
    pool_outputs = pool.map(functools.partial(
        experiment_sample, n=n, player=player, p_min_depth=p_min_depth, p_max_depth=p_max_depth), range(samples))
    pool.close()
    pool.join()

    results = {
        -1: pool_outputs.count(-1),
        0: pool_outputs.count(0),
        1: pool_outputs.count(1)
    }
    return results

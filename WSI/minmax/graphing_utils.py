from matplotlib import pyplot as plt
from multiprocess_experiment import compute_experiment
from constants import P_MIN, P_MAX, NO_PLAYER, PLAYER_STR
import numpy as np
from minmax import minmax_competition
import time
from game_board import BoardState


def graph_depth_comparisons(jobs, samples, n):
    plt.rcParams["figure.figsize"] = 7, 5 * len(jobs)
    fig, axes = plt.subplots(len(jobs), 1)

    if not type(axes) == np.ndarray:
        axes = list([axes])
    else:
        axes = axes.flatten()

    fig.suptitle(
        f"Results of {n}x{n} games from MIN player's perspective over {samples} samples")

    # For each job
    for i, job in enumerate(jobs):
        d_min, player = job
        d_maxs = jobs[job]

        axes[i].set_title(
            f"...given d_min = {d_min} and some d_max with {PLAYER_STR[player]} starting the game")
        axes[i].set_xlabel("d_max")

        Y_wins = []
        Y_ties = []
        Y_defeats = []
        X = []

        # For each depth in job (each bar in graph)
        for d_max in d_maxs:
            X.append(d_max)
            results = compute_experiment(samples, n, player, d_min, d_max)
            if sum(results.values()) != samples:
                raise Exception("SYNC ERROR")
            Y_wins.append(results[P_MIN])
            Y_ties.append(results[NO_PLAYER])
            Y_defeats.append(results[P_MAX])

        # All data collected, graph it
        X = list(map(str, X))
        Y_wins = np.array(Y_wins)
        Y_ties = np.array(Y_ties)
        Y_defeats = np.array(Y_defeats)

        axes[i].bar(X, Y_defeats, color="r", label="loss")
        axes[i].bar(X, Y_ties, bottom=Y_defeats, color="y", label="tie")
        axes[i].bar(X, Y_wins, bottom=Y_defeats + Y_ties, color="g", label="win")

        # Add labels for each bar
        for bar in axes[i].patches:
            font_size = 10
            value = bar.get_height()
            text = f"{round(bar.get_height() / samples * 100, 1)}%"
            x_pos = bar.get_x() + bar.get_width() / 2
            y_pos = max(0, min(samples, value / 2 + bar.get_y()))
            if value == 0:
                continue

            axes[i].text(
                x_pos,
                y_pos,
                text,
                ha='center',
                color='k',
                weight='bold',
                size=font_size
            )

        axes[i].legend()

    # plt.tight_layout()
    plt.show()


def graph_run_times(ns: list, depths: list, samples: int):
    times = []
    X = []
    for n, depth in zip(ns, depths):
        t_start = time.time()
        for i in range(samples):
            b = BoardState(n)
            minmax_competition(b, P_MIN, depth, depth)
            del b
        t_end = time.time()
        times.append((t_end - t_start)/samples)
        X.append(f"{n}x{n}@d={depth}")

    plt.rcParams["figure.figsize"] = 7, 5
    plt.bar(X, times)
    plt.tick_params('x', labelrotation=45)
    plt.ylabel("Execution time (s)")
    plt.title(f"Average execution time of 1 full game with given parameters over {samples} samples")
    plt.show()

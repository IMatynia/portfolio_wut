from evolutionary_algorithm import compute_evolution
from mutation import mutate_gauss_and_crossbreed_singlepoint
from ruletka import reproduce_ruletka_style
from simulation import simulate_factory_locations, evaluate_factory_score
from population import Population
from matplotlib import pyplot as plt
from matplotlib.ticker import MaxNLocator
import matplotlib as mpl
import numpy as np


def plot_evolution_performance_per_sigma(N: int, limit_kratowy: float, sigmas: list, iter: int, n_samples: int = 20):
    X = np.arange(iter)
    OPTIMUM = evaluate_factory_score(1, 1)

    plt.rcParams["figure.figsize"] = 10, 5
    fig, axes = plt.subplots(2, 3)
    axes = axes.flatten()

    fig.suptitle(
        f"Average score from {n_samples} runs of the algorith with given sigma over {iter} generations (more is better)")
    AX_Y_MIN = 17
    AX_Y_MAX = 24

    average_solutions = []

    for i, sigma in enumerate(sigmas):
        best_scores_averages = np.zeros(iter)
        temp_avg_solution = 0
        for _ in range(n_samples):
            best_score, _, best_scores, _, _ = compute_evolution(
                Population.generate(N, limit_kratowy),
                simulate_factory_locations,
                reproduce_ruletka_style,
                mutate_gauss_and_crossbreed_singlepoint,
                sigma,
                iter
            )
            best_scores_averages += np.array(best_scores)
            temp_avg_solution += best_score

        temp_avg_solution /= n_samples
        best_scores_averages /= n_samples

        average_solutions.append(temp_avg_solution)

        axes[i].plot(X, best_scores_averages)
        axes[i].set_title(f"Sigma {sigma}")
        axes[i].set_ybound((AX_Y_MIN, AX_Y_MAX))
        axes[i].set_ylim((AX_Y_MIN, AX_Y_MAX))
        axes[i].yaxis.set_major_locator(MaxNLocator((AX_Y_MAX-AX_Y_MIN)//2))
        axes[i].grid(True)

    plt.tight_layout()
    plt.show()

    plt.bar(list(map(str, sigmas)), OPTIMUM-np.array(average_solutions))
    plt.ylabel("Difference to optimum")
    plt.xlabel("Sigma")
    plt.title("Average solution accuracy (less is better)")
    plt.yscale("log")
    plt.show()


def plot_evolution_performance_per_n(Ns: list, limit_kratowy: float, sigma: float, iter: int, n_samples: int = 20):
    X = np.arange(iter)
    OPTIMUM = evaluate_factory_score(1, 1)

    plt.rcParams["figure.figsize"] = 10, 5
    fig, axes = plt.subplots(2, 2)
    axes = axes.flatten()

    fig.suptitle(
        f"Average score from {n_samples} runs of the algorith with given population size over {iter} generations (more is better)")
    AX_Y_MIN = 10
    AX_Y_MAX = 25

    average_solutions = []

    for i, n in enumerate(Ns):
        best_scores_averages = np.zeros(iter)
        temp_avg_solution = 0
        for _ in range(n_samples):
            best_score, _, best_scores, _, _ = compute_evolution(
                Population.generate(n, limit_kratowy),
                simulate_factory_locations,
                reproduce_ruletka_style,
                mutate_gauss_and_crossbreed_singlepoint,
                sigma,
                iter
            )
            best_scores_averages += np.array(best_scores)
            temp_avg_solution += best_score

        temp_avg_solution /= n_samples
        best_scores_averages /= n_samples

        average_solutions.append(temp_avg_solution)

        axes[i].plot(X, best_scores_averages)
        axes[i].set_title(f"N = {n}")
        axes[i].set_ybound((AX_Y_MIN, AX_Y_MAX))
        axes[i].set_ylim((AX_Y_MIN, AX_Y_MAX))
        axes[i].yaxis.set_major_locator(MaxNLocator((AX_Y_MAX-AX_Y_MIN)//2))
        axes[i].grid(True)

    plt.tight_layout()
    plt.show()

    plt.bar(list(map(str, Ns)), OPTIMUM-np.array(average_solutions))
    plt.ylabel("Difference to optimum")
    plt.xlabel("N")
    plt.title("Average solution accuracy (less is better)")
    plt.yscale("log")
    plt.show()
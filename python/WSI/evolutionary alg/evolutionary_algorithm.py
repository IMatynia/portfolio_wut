import numpy as np
from population import Population


def compute_evolution(population: Population, score_evaluator, reproduction, mutation_and_crossbreed, sigma: float, max_iterations: int, cross_breed_chance: float = 0.9):
    score_evaluator(population)
    best_score, best_specimen = population.find_best()

    score_quartiles_over_time = []
    best_scores = []
    best_specimens = []

    for i in range(max_iterations):
        reproduction(population)
        mutation_and_crossbreed(population, sigma, cross_breed_chance)
        score_evaluator(population)
        best_score_temp, best_specimen_temp = population.find_best()

        # Current top score for graphing
        best_scores.append(best_score_temp)
        best_specimens.append(best_specimen_temp)
        
        # Store quartiles for graphing
        s_scores = sorted(population.get_scores(), reverse=True)
        score_quartiles_over_time.append(
            s_scores[::int(np.ceil(len(s_scores)/4))]
        )

        if best_score < best_score_temp:
            best_score = best_score_temp
            best_specimen = best_specimen_temp
        

    return best_score, best_specimen, best_scores, best_specimens, score_quartiles_over_time

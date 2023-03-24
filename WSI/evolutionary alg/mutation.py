import numpy as np
from population import Population
import random


def mutate_gauss_and_crossbreed_singlepoint(population: Population, sigma: float, cross_breed_chance: float):
    random.shuffle(population.specimen)

    # Crossbreed
    offset = len(population.specimen)//2
    for i in range(offset):
        if random.random() < cross_breed_chance:
            temp = population.specimen[i][0]
            population.specimen[i][0] = population.specimen[i+offset][0]
            population.specimen[offset][0] = temp

    # Mutate
    mutagen = np.random.normal(scale=sigma, size=[len(population.specimen), 2])
    population.specimen += mutagen

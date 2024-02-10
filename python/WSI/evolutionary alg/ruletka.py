import random
from population import Population


def reproduce_ruletka_style(population: Population):
    pop_size = len(population.specimen)
    probs = population.get_scores() / sum(population.get_scores())
    new_pop = random.choices(population.specimen, k=pop_size, weights=probs)
    population.specimen = new_pop

import numpy as np
from population import Population

MAX_COST_VALUE = 20+10+5+10


def evaluate_factory_score(x, y):
    cost = (MAX_COST_VALUE +
            np.expm1(-abs(x-1)-abs(y-1)) * 20 +
            np.expm1(-abs(x+0.5)-abs(y-1)) * 10 +
            np.expm1(-abs(x+1)-abs(y+0.5)) * 5 +
            np.expm1(-abs(x-1)-abs(y+1)) * 10)
    return cost


def simulate_factory_locations(factory_proposals: Population):
    location_proposals = factory_proposals.specimen
    x = location_proposals[:, 0]
    y = location_proposals[:, 1]
    cost = (MAX_COST_VALUE +
            np.expm1(-abs(x-1)-abs(y-1)) * 20 +
            np.expm1(-abs(x+0.5)-abs(y-1)) * 10 +
            np.expm1(-abs(x+1)-abs(y+0.5)) * 5 +
            np.expm1(-abs(x-1)-abs(y+1)) * 10)
    factory_proposals.set_scores(cost)

"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

from dataclasses import dataclass
from typing import Optional

from src.qlearning.qlearning import Qlearning
from src.qlearning.qlearningenvironment import QLearningEnvironment


@dataclass
class Experiment:
    name: str
    params: dict
    result: Optional[dict] = None


@dataclass
class RunnableExperiment(Experiment):
    def run(self):
        print(f"Running esperiment {self.name}")
        self._run_full(**self.params)
        print(f"Experiment {self.name} finished")
        return self.result

    def _run_full(self, gamma, epsilon, beta, episodes, steps, state_space, action_space, function,
                  iterations_per_step):
        environment = QLearningEnvironment(state_space, action_space, function, iterations_per_step)
        agent = Qlearning(gamma, epsilon, beta, state_space, action_space, episodes + 1)
        agent.learn(episodes, steps, environment, None)
        self.result = agent.plotting_data

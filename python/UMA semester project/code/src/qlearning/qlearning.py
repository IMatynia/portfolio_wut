"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

from src.qlearning.lineardiscretisation import *
from src.qlearning.qlearningenvironment import QLearningEnvironment


class Qlearning:
    def __init__(self, gamma: float, epsilon: float, beta: float, state_space: StateSpaceLinearDescretisation,
                 action_space: ActionSpaceLiearDescretisation, policy_caching_frequency: int):
        """
        Initialize the Qlearning
        :param gamma: how far into the future should you look (big gamma, look far into the future)
        :param epsilon: exploration rate (big epsilon, high exploration)
        :param beta: determines the impact of each step on the policy (big beta, big impact)
        """
        self.state_space = state_space
        self.action_space = action_space
        self.policy = np.zeros((self.state_space.get_shape(), self.action_space.get_shape()), dtype=np.float32)
        self.gamma = gamma
        self.epsilon = epsilon
        self.beta = beta
        self.plotting_data = {}
        self.flush_plotting_data()
        self.policy_caching_frequency = policy_caching_frequency

    def flush_plotting_data(self):
        self.plotting_data = {
            "states": {},
            "actions": {},
            "rewards": {},
            "func_calls": {},
            "best_specimen_val": {},
        }

    def _add_datapoint(self, episode: int, step: int, state: tuple, action: tuple, reward: float, func_calls: int,
                       best_specimen: float):
        self.plotting_data["states"][(episode, step)] = state
        self.plotting_data["actions"][(episode, step)] = action
        self.plotting_data["rewards"][(episode, step)] = reward
        self.plotting_data["func_calls"][(episode, step)] = func_calls
        self.plotting_data["best_specimen_val"][(episode, step)] = best_specimen

    def _choose_action(self, state_representation: tuple):
        """
        :param state_representation: current state
        :return: Representation from action_representation
        """
        if np.random.random() < self.epsilon:
            return self.action_space.get_random()
        else:
            state_action_policy = self.policy[state_representation]
            return np.argmax(state_action_policy)

    def predict_action(self, state_representation: tuple):
        """
        Use the policy to get the best action for a given state
        :param state_representation: current state
        :return: action chosen by the policy
        """
        state_action_policy = self.policy[state_representation]
        return np.argmax(state_action_policy)

    def learn(self, episode_count: int, steps_per_episode: int, environment: QLearningEnvironment,
              cache_folder: str = None,
              friendly_name: str = None):
        for episode in range(1, episode_count + 1):
            environment.reset_environment()
            next_state = self.state_space.get_state_representation(*environment.get_initial_state())
            for step in range(1, steps_per_episode + 1):
                current_state = next_state

                action = self._choose_action(current_state)
                action_actual = self.action_space.get_action_from_representation(action)

                reward, next_state_actual = environment.do_action(action_actual)
                next_state = self.state_space.get_state_representation(*next_state_actual)

                # update policy
                self.policy[current_state, action] = (
                        (1 - self.beta) * self.policy[current_state, action] +
                        self.beta * (reward + self.gamma * np.max(self.policy[next_state]))
                )

                self._add_datapoint(
                    episode,
                    step,
                    next_state_actual,
                    action_actual,
                    reward,
                    environment.function.n_calls,
                    np.min(environment.differential_evolution.old_func_values)
                )
            if cache_folder is not None and episode % self.policy_caching_frequency == 0:
                self.save_policy_mid_learning(cache_folder, friendly_name, episode)

    def save_policy_mid_learning(self, cache_folder: str, friendly_name: str, episode: int):
        cache_file = f"{cache_folder}/{friendly_name};{episode}"
        np.save(cache_file, self.policy)

    def read_policy(self, cache_file: str):
        self.policy = np.load(cache_file)

import copy
import functools

import multiprocess
import numpy as np
import random
import gymnasium as gym
from matplotlib import pyplot as plt


class Agent:
    def __init__(self, ag_number: int, epsilon: float, learning_rate: float, discount: float, environment: gym.Env):
        self._num = ag_number
        self._env = environment
        self._epsilon = epsilon
        self._learning_rate = learning_rate
        self._discount = discount
        self._q_values = np.zeros((self._env.observation_space.n, self._env.action_space.n))

    def get_next_action(self, env_state: int, action_mask):
        if random.random() < self._epsilon:
            return self._env.action_space.sample(action_mask)
        else:
            return np.argmax(np.where(action_mask, self._q_values[env_state], -np.inf))

    def update_q_function(self, current_state: int, action: int, next_state: int, reward: float):
        q_value_change = (reward + self._discount * np.max(self._q_values[next_state]) - self._q_values[current_state][
            action])
        self._q_values[current_state][action] += self._learning_rate * q_value_change

    def train_agent(self, n_of_episodes: int, data_sparseness: int) -> list:
        saved_policies = []
        for ep in range(n_of_episodes):
            if ep % data_sparseness == 0:
                saved_policies.append(np.copy(self._q_values))
            episode_ended = False
            env_state, info = self._env.reset()
            while not episode_ended:
                action = self.get_next_action(env_state, info["action_mask"])
                next_env_state, reward, terminated, truncated, info = self._env.step(action)
                self.update_q_function(env_state, action, next_env_state, float(reward))
                env_state = next_env_state
                episode_ended = truncated or terminated
        return saved_policies

    def get_policy(self):
        return self._q_values


def run_single_episode(i: int, env_name: str, discount: float, policy: np.array) -> list:
    success = 0
    episode_ended = False
    total_reward = 0
    accumulated_discount = 1
    env = gym.make(env_name)
    env_state, info = env.reset(seed=i)
    while not episode_ended:
        action_mask = info["action_mask"]
        action = np.argmax(np.where(action_mask, policy[env_state], -np.inf))
        next_env_state, reward, terminated, truncated, info = env.step(action)
        total_reward += float(reward) * accumulated_discount
        accumulated_discount *= discount
        if float(reward) > 0:
            success = 1
        env_state = next_env_state
        episode_ended = truncated or terminated
    return list([total_reward, success])


def rate_policy(policy: np.array, env_name: str, samples: int, discount: float) -> tuple[list, float]:
    successes = 0
    all_rewards = []

    pool_size = multiprocess.cpu_count() * 2
    pool = multiprocess.Pool(
        processes=pool_size,
        maxtasksperchild=2,
    )
    pool_outputs = pool.map(
        functools.partial(
            run_single_episode,
            env_name=env_name,
            discount=discount,
            policy=policy),
        range(samples)
    )

    pool.close()
    pool.join()

    pool_outputs = np.array(pool_outputs)

    all_rewards = pool_outputs[:, 0]
    successes = sum(pool_outputs[:, 1])
    return all_rewards, successes / samples


def main():
    pass


if __name__ == "__main__":
    main()

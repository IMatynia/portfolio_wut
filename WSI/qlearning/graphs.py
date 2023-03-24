import functools
import gymnasium as gym
import multiprocess
from qlearning import Agent, rate_policy
from matplotlib import pyplot as plt
import numpy as np

RATE_SAMPLES = 500


def train_one_agent(ag_num: int, env_name: str, epsilon, learning_rate, discount, train_episodes, data_sparseness):
    env = gym.make(env_name)
    agent = Agent(ag_num, epsilon, learning_rate, discount, env)
    return agent.train_agent(train_episodes, data_sparseness)


def plot_hiperparam_comparisons(agents_per_hiperparms: int,
                                data_sparseness: int,
                                train_episodes: int,
                                epsilon_list: list[float],
                                learning_rate_list: list[float],
                                horizon_list: list[int],
                                env_name: str,
                                labels: list,
                                graph_title: str):
    fig, (ax1, ax2) = plt.subplots(2, 1, sharex="col")
    X = np.arange(train_episodes // data_sparseness) * data_sparseness
    hip_param_set_num = 0
    for epsilon, learning_rate, horizon, graph_label in zip(epsilon_list,
                                                            learning_rate_list,
                                                            horizon_list,
                                                            labels):
        discount = 1 - 1 / horizon
        pool_size = multiprocess.cpu_count() * 2
        pool = multiprocess.Pool(
            processes=pool_size,
            maxtasksperchild=2,
        )
        pool_outputs = pool.map(
            functools.partial(
                train_one_agent,
                env_name=env_name,
                epsilon=epsilon,
                learning_rate=learning_rate,
                discount=discount,
                train_episodes=train_episodes,
                data_sparseness=data_sparseness
            ),
            range(agents_per_hiperparms)
        )

        pool.close()
        pool.join()

        pool_outputs = np.array(pool_outputs)

        reward_avg = []
        reward_std = []
        success_rates = []
        for i in range(len(pool_outputs[0])):
            all_agent_rewards = []
            all_success_rates = []
            for policy in pool_outputs[:, i]:
                rewards, success_rate = rate_policy(policy, env_name, RATE_SAMPLES, discount)
                all_success_rates.append(success_rate)
                all_agent_rewards.extend(rewards)
            reward_avg.append(np.average(all_agent_rewards))
            reward_std.append(np.std(all_agent_rewards))
            success_rates.append(np.average(all_success_rates))

        offset = data_sparseness * hip_param_set_num / len(epsilon_list) * 0.75
        ax1.plot(X, success_rates, label=graph_label)
        ax2.errorbar(X + offset, reward_avg, reward_std, fmt=".", label=graph_label)
        hip_param_set_num += 1
    ax1.legend()
    ax2.legend()
    ax1.set_title("Szansa na sukces")
    ax2.set_title("Średnia nagroda")
    ax2.set_xlabel("Epizod")
    fig.suptitle(graph_title + f" (dane z {agents_per_hiperparms} wywołań)")
    plt.show()

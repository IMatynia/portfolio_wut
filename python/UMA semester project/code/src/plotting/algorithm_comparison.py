"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

import multiprocessing
import pickle

from matplotlib import pyplot as plt

from src.differential_evolution.differential_evolution import *
from src.qlearning.qlearning import *

HIGH = 1
LOW = 0


class FunctionTargetPairGraph:
    def __init__(self):
        # Raw DE (no RL) run hyper parameters
        self.raw_cr_value = 0.75
        self.raw_crossover = 0
        self.raw_selection = 0
        self.raw_f_value = 1.75

        self.dimentions = [10, 30, 50]
        self.functions = list(range(2, 31, 2))
        # self.functions = [2, 4, 6, 8, 10, 12, 14, 16, ]
        self.graph_repeats = 30
        self.evoliton_steps = 150
        self.policy_folder = "./rl_model_snapshots"
        self.training_repeats_to_graph = [5, 10, 20, 29]
        self.calls_needed_to_train_policy = {}
        self.iterations_per_step_in_rl = 5

        self.policy_episode = 2
        self.action_space_final = ActionSpaceLiearDescretisation(
            [0.6, 0.75, 0.9], [0.6, 0.75, 1.0, 1.2, 1.8]
        )

    def prep_data(self):
        for dimension in self.dimentions:
            print(f"Dimention: {dimension}")
            state_space_final = StateSpaceLinearDescretisation(HIGH, 10, 5, dimension)

            algorighm_results = {}
            num_processes = multiprocessing.cpu_count()
            res = []
            with multiprocessing.Pool(processes=num_processes) as pool:
                res = pool.starmap(self.process_function_dimention,
                                   zip([dimension for _ in self.functions],
                                       [function_index for function_index in self.functions],
                                       [state_space_final for _ in self.functions])),

            res = res[0]
            for result in res:
                algorighm_results.update(result)

            # Data collected, average out and squash to 0;1 scale
            mins = {}
            maxs = {}
            # get maxs and mins
            print("Getting maxs and mins")
            for function_index, policy_rep in algorighm_results:
                gathered_values = algorighm_results[function_index, policy_rep]

                if function_index not in mins:
                    mins[function_index] = 1e16
                if function_index not in maxs:
                    maxs[function_index] = 0

                mins[function_index] = min(min(gathered_values), mins[function_index])
                maxs[function_index] = max(max(gathered_values), maxs[function_index])

            final_values = {}
            # scale graphs and save final values
            print("Scaling values")
            for function_index, policy_rep in algorighm_results:
                gathered_values = algorighm_results[function_index, policy_rep]
                scaled_values = (gathered_values - mins[function_index]) / (maxs[function_index] - mins[function_index])

                # collect averages in final values
                if policy_rep not in final_values:
                    final_values[policy_rep] = scaled_values / len(self.functions)
                else:
                    final_values[policy_rep] += scaled_values / len(self.functions)

            # finished data gather for all consideted policies, raw DE for some dimention.
            functions_string = ", ".join(map(str, self.functions))
            self.plot_target_function_pair_plot(
                f"Compare algorithms on functions {functions_string} in {dimension} dimentions",
                final_values)

    def process_function_dimention(self, dimension, function_index, state_space_final):
        algorighm_results = {}
        # Set up vars for dimention-function pair
        function = Function(function_index, dimension, LOW, HIGH)
        env = QLearningEnvironment(state_space_final, self.action_space_final,
                                   function,
                                   self.iterations_per_step_in_rl)
        # Run on policies
        for training_repeat in self.training_repeats_to_graph:
            agent = Qlearning(0, 0, 0, state_space_final, self.action_space_final, 10000)
            agent.read_policy(
                self.get_policy_file_name(training_repeat, dimension, 23, self.policy_episode))

            average_vals = None
            for graphing_repeat in range(self.graph_repeats):
                print(f"F{function_index}, Calculating for Policy: {training_repeat}, Repetition: {graphing_repeat}")
                agent.learn(
                    1, self.evoliton_steps, env
                )
                best_vals = sorted(agent.plotting_data["best_specimen_val"].items())
                agent.flush_plotting_data()
                only_vals_over_iterations = np.array([val[1] for val in best_vals])
                if average_vals is None:
                    average_vals = only_vals_over_iterations / self.graph_repeats
                else:
                    average_vals += only_vals_over_iterations / self.graph_repeats

            # save result
            algorighm_results[(function_index, training_repeat)] = average_vals  # Run on raw DE
        average_vals = None
        for rep in range(self.graph_repeats):
            print(f"F{function_index}, Calculating for raw DE, Repetition: {rep}")
            de = DifferentialEvolution(function, self.iterations_per_step_in_rl)
            func_values = []
            for step in range(self.evoliton_steps):
                de.do_evolution(self.raw_f_value, self.raw_selection, self.raw_crossover, self.raw_cr_value)
                func_values.append(np.min(de.old_func_values))

            if average_vals is None:
                average_vals = np.array(func_values) / self.graph_repeats
            else:
                average_vals += np.array(func_values) / self.graph_repeats
        algorighm_results[(function_index, None)] = average_vals
        return algorighm_results

    def plot_target_function_pair_plot(self, name: str, values):
        fig, ax = plt.subplots(figsize=(6, 4))
        fig.suptitle(name)

        ax.set_xlabel("Step")
        ax.set_ylabel("Relative fitness")

        for algorightm in values:
            final_value = values[algorightm]
            alg_name = ""

            if algorightm is None:
                alg_name = f"Raw DE/rand/exp/2 F={self.raw_f_value} CR={self.raw_cr_value}"
            else:
                alg_name = f"QL+DE, training rep: {algorightm}"
            print(f"Plotting {alg_name}")
            ax.plot(final_value, label=alg_name)
        ax.legend()
        fig.savefig(name + "_fig.png", dpi=150, bbox_inches='tight')

    def get_policy_file_name(self, repeat, dimention, function, episode):
        return f"{self.policy_folder}/{repeat};{dimention};{function};{episode}.npy"

    def get_policy_statistics(self, repeat, dimention, function):
        file = f"{self.policy_folder}/{repeat};{dimention};{function}_stats.bin"
        obj = None
        with open(file, "rb") as f:
            obj = pickle.load(f)
        return obj

    def run(self):
        self.prep_data()

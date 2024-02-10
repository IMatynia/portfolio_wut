"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

from src.plotting.AbstractPlotElement import AbstractPlotElement


class RewardsOverEpisodesAndSteps(AbstractPlotElement):
    def plot(self):
        x_values_saved = None
        for experiment in self.experiments:
            data = experiment.result
            points = sorted(data["rewards"].items())
            x_values, y_values = zip(*points)
            self.axis.scatter(range(len(x_values)), y_values, label=experiment.name)
            if x_values_saved is None:
                x_values_saved = x_values

        # Shared labels and axis
        x_values = list(map(lambda timestamp: f"E:{timestamp[0]} S:{timestamp[1]}", x_values_saved))
        self.axis.set_xticks(range(len(x_values)))
        self.axis.set_xticklabels(x_values, rotation=45)

        self.axis.set_title("Rewards over the raining process")
        self.axis.set_xlabel("Timestamp")
        self.axis.set_ylabel("Reward")

        self.axis.legend()


class RewardsOverEpisodesAndStepsAveragedOverExperiments(AbstractPlotElement):
    def plot(self):
        final_pairs = {}
        for experiment in self.experiments:
            data = experiment.result["rewards"]
            for timestamp in data:
                if timestamp not in final_pairs:
                    final_pairs[timestamp] = 0.0
                final_pairs[timestamp] += data[timestamp]/len(self.experiments)

        points = sorted(final_pairs.items())
        x_values, y_values = zip(*points)
        self.axis.scatter(range(len(x_values)), y_values, label=self.title)
        # Shared labels and axis
        x_values = list(map(lambda timestamp: f"E:{timestamp[0]} S:{timestamp[1]}", x_values))
        self.axis.set_xticks(range(len(x_values)))
        self.axis.set_xticklabels(x_values, rotation=45)

        self.axis.set_title(self.title)
        self.axis.set_xlabel("Timestamp")
        self.axis.set_ylabel("Reward")

        self.axis.legend()

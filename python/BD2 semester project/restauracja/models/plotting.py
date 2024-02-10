from dataclasses import dataclass
import plotly.graph_objects as go
import plotly.io
from django.http import QueryDict
from .plot_variables import PlotVariable, VarProfit, VarExpenses, VarCustomers, VarIncome


@dataclass
class PlotPreset(PlotVariable):
    variables: list[PlotVariable]

    def plot(self, fig: go.Figure, grain: str):
        for var in self.variables:
            var.plot(fig, grain)

    def __eq__(self, other: "PlotPreset"):
        if other:
            return set(self.variables) == set(other.variables)
        else:
            return False


class PlotGenerator:
    VARS: dict[str, PlotVariable] = {
        "profits": VarProfit(),
        "expenses": VarExpenses(),
        "customers": VarCustomers(),
        "income": VarIncome(),
    }

    PRESETS: dict[str, PlotPreset] = {
        "profits_time": PlotPreset("Profits over time", "profits_time", [VarProfit()]),
        "income_vs_expenses": PlotPreset("Income versus expenses", "income_vs_expenses",
                                         [VarIncome(), VarExpenses()])
    }

    def __init__(self):
        pass

    def parse_presets(self, preset_code: str):
        if not preset_code in self.PRESETS:
            raise Exception(f"Invalid preset {preset_code}")

        return self.PRESETS[preset_code]

    def parse_args(self, arguments: QueryDict):
        used_vars = []
        for var_code in self.VARS:
            temp_var = arguments.get(var_code)
            if temp_var == 'on':
                used_vars.append(self.VARS[var_code])

        if len(used_vars) == 0:
            return None

        return PlotPreset("Custom plot", "custom", used_vars)

    def get_HTML_plot(self, preset: PlotPreset, grain: str):
        layout = go.Layout(
            title=preset.display_name,
            yaxis=dict(title="Money (PLN)"),
            yaxis2=dict(title="Units", overlaying="y", side="right"),
            yaxis3=dict(title="Kilograms", overlaying="y", anchor="free", autoshift=True),
            xaxis=dict(title="Time")
        )
        fig = go.Figure(layout=layout)
        preset.plot(fig, grain)

        return plotly.io.to_html(fig)

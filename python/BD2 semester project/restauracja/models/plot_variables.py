from dataclasses import dataclass
import plotly.graph_objects as go
from restauracja.models.queries import profit_granularity, expenses_granularity, customers_granularity, time_series_aggregated, income_granularity

MONEY_AXIS = "y1"
UNIT_AXIS = "y2"
WEIGHT_AXIS = "y3"


@dataclass
class PlotVariable:
    display_name: str
    code: str

    def __eq__(self, other: "PlotVariable"):
        return self.code == other.code

    def __hash__(self):
        return hash(self.code)

    def plot(self, fig: go.Figure, grain: str):
        raise NotImplemented


class VarProfit(PlotVariable):
    def __init__(self):
        super().__init__("profits", "profits")

    def plot(self, fig: go.Figure, grain: str):
        profit = profit_granularity("All")
        aggregated_profit = time_series_aggregated(grain, profit)
        Y = [p for p, _ in aggregated_profit]
        X = [d for _, d in aggregated_profit]

        trace = go.Scatter(
            x=X,
            y=Y,
            name="Profits (PLN)",
            yaxis=MONEY_AXIS
        )

        fig.add_trace(trace)


class VarExpenses(PlotVariable):
    def __init__(self):
        super().__init__("expenses", "expenses")

    def plot(self, fig: go.Figure, grain: str):
        expenses = expenses_granularity("All")
        aggregated_expenses = time_series_aggregated(grain, expenses)

        Y = [p for p, _ in aggregated_expenses]
        X = [d for _, d in aggregated_expenses]

        trace = go.Scatter(
            x=X,
            y=Y,
            name="Expenses (PLN)",
            yaxis=MONEY_AXIS
        )

        fig.add_trace(trace)


class VarCustomers(PlotVariable):
    def __init__(self):
        super().__init__("customers", "customers")

    def plot(self, fig: go.Figure, grain: str):
        customers = customers_granularity("All")
        aggregated_customers = time_series_aggregated(grain, customers)

        Y = [c for c, _ in aggregated_customers]
        X = [d for _, d in aggregated_customers]

        trace = go.Scatter(
            x=X,
            y=Y,
            name="Customers",
            yaxis=UNIT_AXIS
        )

        fig.add_trace(trace)


class VarIncome(PlotVariable):
    def __init__(self):
        super().__init__("income", "income")

    def plot(self, fig: go.Figure, grain: str):
        income = income_granularity("All")
        aggregated_expenses = time_series_aggregated(grain, income)

        Y = [p for p, _ in aggregated_expenses]
        X = [d for _, d in aggregated_expenses]

        trace = go.Scatter(
            x=X,
            y=Y,
            name="Income (PLN)",
            yaxis=MONEY_AXIS
        )

        fig.add_trace(trace)
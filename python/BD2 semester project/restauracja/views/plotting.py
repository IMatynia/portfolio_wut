from typing import Optional
import plotly.io
import plotly.graph_objects as go

from django.http import HttpRequest
from django.views import View
from django.shortcuts import render
from .primary_nav_items import DEFAULT_NAV
from ..models.plotting import PlotGenerator, PlotPreset
from ..models.queries import TIME_GRANULARITY, DEFAULT_GRAIN


class PlottingView(View):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self._plot_gen = PlotGenerator()

    def get(self, request: HttpRequest, *args, **kwargs):
        preset_code = request.GET.get("preset")
        chosen_grain = request.GET.get("grain")
        preset: Optional[PlotPreset] = None

        if not chosen_grain:
            chosen_grain = DEFAULT_GRAIN

        if preset_code:
            preset = self._plot_gen.parse_presets(preset_code)
        else:
            preset = self._plot_gen.parse_args(request.GET)

        if preset:
            plot_html = self._plot_gen.get_HTML_plot(preset, chosen_grain)
        else:
            plot_html = None

        context = {
            "plot": plot_html,
            "plot_preset": preset,
            "nav_items": DEFAULT_NAV,
            "variables": PlotGenerator.VARS,
            "presets": PlotGenerator.PRESETS,
            "chosen_grain": chosen_grain,
            "granularities": list(TIME_GRANULARITY.keys())
        }
        return render(request, "plots/index.html", context=context)

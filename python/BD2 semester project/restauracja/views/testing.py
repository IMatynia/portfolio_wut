import plotly.io
from django.http import HttpResponse, HttpRequest
from django.views import View
import numpy as np
import plotly.graph_objects as go


class TestView(View):
    def get(self, request: HttpRequest, *args, **kwargs):
        out = ":)"

        return HttpResponse(out)

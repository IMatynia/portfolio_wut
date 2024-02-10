from django.http import HttpRequest
from django.views import View
from django.shortcuts import render
from .primary_nav_items import DEFAULT_NAV
from ..models.queries import get_order_backlog


class OrderBacklog(View):
    def get(self, request: HttpRequest):
        backlog = get_order_backlog()
        context = {
            "nav_items": DEFAULT_NAV,
            "order_backlog": backlog
        }
        return render(request, "order backlog/index.html", context)


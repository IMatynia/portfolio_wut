import datetime
from dataclasses import dataclass

from django.http import HttpRequest, HttpResponse
from django.views import View
from django.shortcuts import render
from .primary_nav_items import DEFAULT_NAV
from ..models.queries import reservations_at, get_order_backlog
from ..models.models import (Tables, Offers, Employees)

from django.utils import timezone


@dataclass
class TableCompiled:
    table: Tables
    state: str


class TableManager(View):
    def get(self, request: HttpRequest, *args, **kwargs):
        selected_date = request.GET.get("time")
        if not selected_date:
            now = timezone.localtime(timezone.now())
        else:
            now = datetime.datetime.fromisoformat(selected_date)

        reservations = reservations_at(now)
        tables = Tables.objects.all()
        order_comp = get_order_backlog()

        tables_reserved = set([r.tables.id for r in reservations])
        tables_occupied = set()
        for o in order_comp:
            for t in o.tables:
                tables_occupied.add(t.id)

        out = []
        for table in tables:
            state = "free"
            if table.id in tables_reserved:
                state = "reserved"
            if table.id in tables_occupied:
                state = "occupied"

            out.append(TableCompiled(
                table,
                state
            ))

        context = {
            "nav_items": DEFAULT_NAV,
            "tables": out,
            "selected_date": selected_date,
            "all_dishes": Offers.objects.all(),
            "all_employees": Employees.objects.all()
        }
        return render(request, "table manager/index.html", context)

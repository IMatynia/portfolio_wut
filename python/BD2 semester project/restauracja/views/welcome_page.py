from django.shortcuts import render
from django.views import View
from restauracja.models.models import Tables
from restauracja.views.primary_nav_items import DEFAULT_NAV

class WelcomePage(View):
    def get(self, request, *args, **kwargs):
        context = {
            "nav_items": DEFAULT_NAV
        }
        return render(request, "welcome/index.html", context)

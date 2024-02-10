from django.http import HttpRequest
from django.views import View
from django.shortcuts import render

from restauracja.models.models import Offers
from restauracja.views.primary_nav_items import DEFAULT_NAV
from restauracja.models.queries import get_menus, get_active_menu_id


class MenuManager:
    class MenusIndex(View):
        def __init__(self, **kwargs):
            super().__init__(**kwargs)

        def get(self, request: HttpRequest):
            all_menus = get_menus()

            # put active on top
            try:
                active_index = [i for i, el in enumerate(all_menus) if el.is_active][0]
                all_menus[0], all_menus[active_index] = all_menus[active_index], all_menus[0]
            except IndexError:
                pass

            context = {
                "nav_items": DEFAULT_NAV,
                "menus": all_menus
            }
            return render(request, "menu/index.html", context=context)

    class EditMenu(View):
        def __init__(self, **kwargs):
            super().__init__(**kwargs)

        def get(self, request: HttpRequest, menu_id: int):
            current_menu = [menu for menu in get_menus() if menu.id == menu_id][0]

            context = {
                "nav_items": DEFAULT_NAV,
                "menu": current_menu,
                "all_dishes": Offers.objects.all()
            }
            return render(request, "menu/edit_menu.html", context=context)


class MenuShow(View):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def get(self, request: HttpRequest, menu_id: int):
        this_menu = [menu for menu in get_menus() if menu.id == menu_id][0]

        offers = {}
        for offer in this_menu.dishes:
            cat = offer.type.name
            if cat in offers:
                offers[cat].append(offer)
            else:
                offers[cat] = [offer]

        context = {
            "nav_items": DEFAULT_NAV,
            "menu": this_menu,
            "offers": offers
        }
        return render(request, "menu/plain_display.html", context=context)


class MenuShowActive(MenuShow):
    def get(self, request: HttpRequest, menu_id=None):
        active_menu_id = get_active_menu_id()
        return super().get(request, active_menu_id)

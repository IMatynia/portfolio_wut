import datetime

from django.http import HttpRequest, JsonResponse, HttpResponseForbidden, HttpResponse
from django.views import View
from restauracja.models.updates import (
    remove_menu, make_menu_active, insert_menu_from_names, add_dish_to_menu, remove_dish_from_menu, insert_orders,
    remove_order, add_reservation
)


class JsonResponseOK(JsonResponse):
    def __init__(self, **kwargs):
        super().__init__({"error": None, **kwargs})


class JsonResponseError(JsonResponse):
    def __init__(self, error: Exception):
        super().__init__({"error": str(error)})


class ApiView(View):
    ALLOWED_ENDPOINTS = [
        'makemenuprimary',
        'removemenu',
        'removeoffer',
        'addmenu',
        'addoffer',
        'addorder',
        'removeorder',
        'addreservation'
    ]

    def get(self, request: HttpRequest, endpoint: str):
        if endpoint in self.ALLOWED_ENDPOINTS:
            endpoint_handle = getattr(self, endpoint)
            return endpoint_handle(request)
        else:
            return HttpResponseForbidden()

    def makemenuprimary(self, request: HttpRequest):
        menu_id = request.GET.get("menu")
        try:
            make_menu_active(menu_id)
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK()

    def removemenu(self, request: HttpRequest):
        menu_id = request.GET.get("menu")
        try:
            remove_menu(menu_id)
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK()

    def addmenu(self, request: HttpRequest):
        menu_name = request.GET.get("name")
        try:
            new_menu = insert_menu_from_names(menu_name, [])
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK(menu_id=new_menu.id)

    def addoffer(self, request: HttpRequest):
        menu_id = request.GET.get("menu")
        offer_id = request.GET.get("offer")

        try:
            add_dish_to_menu(menu_id, offer_id)
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK()

    def removeoffer(self, request: HttpRequest):
        menu_id = request.GET.get("menu")
        offer_id = request.GET.get("offer")

        try:
            remove_dish_from_menu(menu_id, offer_id)
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK()

    def addorder(self, request: HttpRequest):
        offer_ids = request.GET.get("offers")
        table_id = request.GET.get("table")
        assignee_id = request.GET.get("assignee")

        try:
            offer_ids = offer_ids.split(";")
            insert_orders(table_id, assignee_id, offer_ids)
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK()

    def removeorder(self, request: HttpRequest):
        order_id = request.GET.get("order")
        try:
            remove_order(order_id)
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK()

    def addreservation(self, request: HttpRequest):
        table_id = request.GET.get("table")
        reservator_name = request.GET.get("reservator-name")
        res_start = request.GET.get("reservation-start")
        res_end = request.GET.get("reservation-end")
        try:
            add_reservation(table_id, reservator_name, datetime.datetime.fromisoformat(res_start), datetime.datetime.fromisoformat(res_end))
        except Exception as e:
            return JsonResponseError(e)
        else:
            return JsonResponseOK()

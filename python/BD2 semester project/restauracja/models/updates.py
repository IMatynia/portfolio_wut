from datetime import datetime

from django.utils import timezone

from .models import (Offers,
                     Menus,
                     Menusoffers,
                     Customersorders,
                     Customersordersoffers,
                     Customersorderstables,
                     Employees,
                     Tables,
                     Ingredientsoffers,
                     Ingredients,
                     Reservations
                     )


def insert_menu_from_names(menu_name: str, dish_names: list[str]):
    if menu_name == "":
        raise Exception("Menu name cant be empty!")
    dishes = Offers.objects.filter(name__in=dish_names)
    now = datetime.now().date()
    menu = Menus(
        name=menu_name,
        is_active=False,
        month_start=now.month,
        day_start=now.day,
        month_end=now.month,
        day_end=now.day
    )
    menu.save()
    for dish in dishes:
        Menusoffers(menu.id, dish.id).save()
    return menu


def get_menu_dishes(menu_name: str):
    menu = Menus.objects.filter(name=menu_name).first()
    dish_ids = Menusoffers.objects.filter(menu=menu.id)
    return Offers.objects.filter(id__in=dish_ids)


def insert_orders(table_id: str, employee_id: int, dish_ids: list[int]):
    dishes = Offers.objects.filter(id__in=dish_ids)
    total_price = sum([dish.price for dish in dishes])
    employee = Employees.objects.filter(id=employee_id).first()

    order = Customersorders(
        takeout=table_id is None,
        order_time=datetime.now(),
        employee=employee
    )
    order.save()

    for dish in dishes:
        Customersordersoffers(None, order.id, dish.id).save()

    if table_id is not None:
        Customersorderstables(None, order.id, table_id).save()


def insert_dish(dish_name: str, price: float, ingredients: list[Ingredients], quantities: list[int]):
    dish = Offers(dish_name, price)
    dish.save()
    for ingredient, quantity in zip(ingredients, quantities):
        Ingredientsoffers(dish.id, ingredient.id, quantity).save()


def remove_menu(menu_id: int):
    obj = Menus.objects.get(id=menu_id)
    obj.delete()


def make_menu_active(menu_id: int):
    active_menu = Menus.objects.filter(is_active=True)
    for menu in active_menu:
        menu.is_active = False
        menu.save()

    menu = Menus.objects.get(id=menu_id)
    menu.is_active = True
    menu.save()


def add_dish_to_menu(menu_id: int, offer_id: int):
    Menusoffers(menu_id=menu_id, offer_id=offer_id).save()


def remove_dish_from_menu(menu_id: int, offer_id: int):
    obj = Menusoffers.objects.filter(menu_id=menu_id, offer_id=offer_id)
    obj.delete()


def remove_order(order_id: int):
    obj = Customersorders.objects.filter(id=order_id).first()
    obj.finish_time = timezone.localtime(timezone.now())
    obj.save()


def add_reservation(table_id: int, reservator_name: str, res_start: datetime, res_end: datetime):
    if res_start >= res_end:
        raise Exception("Start must come before end")

    if len(reservator_name) == 0:
        raise Exception("Reservator name cant be empty")

    new_res = Reservations(
        tables_id=table_id,
        start_time=res_start,
        end_time=res_end,
        name=reservator_name
    )
    new_res.save()

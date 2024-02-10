from datetime import datetime, timedelta, date

from django.utils import timezone

from restauracja.models.models import (Customersorders,
                                       Customersordersoffers,
                                       Customersorderstables,
                                       Ingredientssuppliersorders,
                                       Ingredients,
                                       Offers,
                                       Employees,
                                       Reservations,
                                       Menus,
                                       Reservations,
                                       Tables,
                                       Menusoffers)
from .database_views import Customerordersfull
from django.db.models import Count, Avg
from dataclasses import dataclass
from django.db import connection


TIME_GRANULARITY = {'Day': timedelta(days=1), 'Week': timedelta(weeks=1), 'Month': timedelta(weeks=4)}
DEFAULT_GRAIN = 'Day'


def time_series_granular(granularity: str, func_during):
    now = datetime.now().date()
    if granularity in TIME_GRANULARITY:
        return func_during(now - TIME_GRANULARITY[granularity], now)
    elif granularity == "Year":
        curr_year = datetime.today().year
        return func_during(datetime(curr_year, 1, 1), now)
    return func_during(datetime.min, datetime.max)

# data -> tuple(float, datetime)
def time_series_aggregated(granularity, data):
    aggregator_functions = {
        'Day': lambda dt: dt.date().day,
        'Week': lambda dt: dt.isocalendar()[1],
        'Month': lambda dt: dt.date().month,
        'Year': lambda dt: dt.date().year,
    }

    aggregated = []
    index_func = aggregator_functions[granularity]
    for entry in data:
        entry = list(entry)
        if not aggregated:
            aggregated.append(entry)
        elif index_func(aggregated[-1][1]) == index_func(entry[1]):
            aggregated[-1][0] += entry[0]
        else:
            aggregated.append(entry)
    return aggregated

def income_granularity(granularity: str):
    return time_series_granular(granularity, ordered_income_during)


def expenses_granularity(granularity: str):
    return time_series_granular(granularity, ordered_expenses_during)


def profit_granularity(granularity: str):
    return time_series_granular(granularity, ordered_profit_during)


def customers_granularity(granularity: str):
    return time_series_granular(granularity, ordered_customers_during)


def orders_during(start: datetime, end: datetime):
    orders = Customersordersoffers.objects.select_related('customer_order', 'offer')
    return orders.order_by("customer_order__order_time").filter(customer_order__order_time__range=(start, end))


def ordered_income_during(start: datetime, end: datetime):
    sorted_ordered = orders_during(start, end)
    return [(order.offer.price, order.customer_order.order_time) for order in sorted_ordered]


def ingredients_ordered_during(start: datetime, end: datetime):
    ingredients_ordered = Ingredientssuppliersorders.objects.all()
    return ingredients_ordered.select_related("supplier_order", "ingredient") \
        .order_by("supplier_order__date") \
        .filter(supplier_order__date__range=(start, end))


def ordered_expenses_during(start: datetime, end: datetime):
    timed_ingredient_orders = ingredients_ordered_during(start, end)
    return [(float(order.ingredient.price) * float(order.quantity), order.supplier_order.date) for order in
            timed_ingredient_orders]


def ordered_profit_during(start: datetime, end: datetime):
    income = list(reversed(ordered_income_during(start, end)))
    expenses = list(reversed(ordered_expenses_during(start, end)))

    profits = []
    total = 0
    while income or expenses:
        if not expenses or (income and income[-1][1] < expenses[-1][1]):
            inc, date = income.pop()
            total += float(inc)
            profits.append((total, date))
        else:
            exp, date = expenses.pop()
            total -= float(exp)
            profits.append((total, date))
    return profits


def ordered_customers_during(start: datetime, end: datetime):
    income = ordered_income_during(start, end)
    return [(1, d) for i, (_, d) in enumerate(income)]


def allergies_per_ingredient():
    '''
    Count the number of allerfies for each ingredient. The returned structure has two fields: name and num_allergies.
    '''
    return Ingredients.objects.annotate(num_allergies=Count('allergiesingredients__allergy'))


def allergies_per_dish():
    '''
    Count the number of allergies for each dish. The returned structure has two fields: name and num_allergies.
    '''
    return Offers.objects.annotate(num_allergies=Count('dishesingredients__ingredient__allergiesingredients__allergy'))


def average_salary_per_employee_type():
    return Employees.objects.values('employeestype__name').annotate(average_salary=Avg('salary'))


def reservations_at(time: datetime):
    return Reservations.objects.filter(start_time__lte=time, end_time__gte=time)


def total_salaries_month():
    return sum([emp.salary for emp in Employees.objects.all()])


@dataclass
class Menu:
    id: int
    name: str
    is_active: bool
    dishes: list[Offers]
    menu_obj: Menus


def get_menus():
    menus_models = Menus.objects.all()
    dishes_menus = Menusoffers.objects.all()

    menus = []

    for menu_model in menus_models:
        dishes_id = dishes_menus.filter(menu=menu_model.id)
        dishes = Offers.objects.filter(id__in=dishes_id.values("offer_id"))
        menus.append(Menu(
            menu_model.id,
            menu_model.name,
            menu_model.id == get_active_menu_id(),
            list(dishes),
            menu_model
        ))

    return menus


def get_active_menu_id():
    with connection.cursor() as cursor:
        now = timezone.localtime(timezone.now())
        month = now.month
        day = now.day
        cursor.execute(f"SELECT get_menu_id_of_date({month}, {day})")  # Replace with your function name
        result = cursor.fetchone()[0]
    return result


@dataclass
class OrderCompiled:
    order: Customersorders
    tables: list[Customersorderstables]
    offers: list[Customersordersoffers]


def get_order_backlog():
    all_orders = Customerordersfull.objects.order_by('order_time')
    all_orders = all_orders.filter(is_finished=False)
    out: list[OrderCompiled] = []
    for order in all_orders:
        offer_ids = Customersordersoffers.objects.filter(customer_order_id=order.id)
        tables_ids = Customersorderstables.objects.filter(customer_order_id=order.id)
        out.append(OrderCompiled(
            order,
            Tables.objects.filter(id__in=tables_ids.values("table_id")),
            Offers.objects.filter(id__in=offer_ids.values("offer_id")),
        ))
    return out

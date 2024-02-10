import datetime

from django.test import TestCase

from django.test import TestCase
from restauracja.models.models import (Customersorders,
                                       Employees,
                                       Employeestypes,
                                       Offers,
                                        Offerstypes,
                                       Menus,
                                       Suppliersorders,
                                       Ingredientssuppliers,
                                       Ingredientssuppliersorders,
                                       Ingredients,
                                       Suppliers,
                                       Addresses,
                                       Countries,
                                       Offers,
                                       Tables,
                                       Reservations)
from restauracja.models.updates import insert_menu_from_names, get_menu_dishes
from restauracja.models.queries import customers_granularity, time_series_aggregated, expenses_granularity, income_granularity
from sqlite3 import IntegrityError


class QueriesTestCase(TestCase):
    def insert_employee(self):
        Employeestypes.objects.create(id=0, name="type", min_salary=1000, max_salary=2000)
        Employees.objects.create(
            id=0,
            name="Name",
            surname="Name",
            salary=1500.0,
            birthdate=datetime.MINYEAR,
            pesel="1234",
            email="mail",
            phone_number="123456789",
            employee_type_id=0
        )

    def insert_customer_orders(self):
        self.create_order(datetime.datetime(2023, 5, 16, 9, 0), 1.0)
        self.create_order(datetime.datetime(2023, 5, 16, 10, 0), 2.0)
        self.create_order(datetime.datetime(2023, 6, 10, 9, 0), 1.0)
        self.create_order(datetime.datetime(2023, 6, 17, 9, 0), 2.0)
        self.create_order(datetime.datetime(2023, 6, 18, 9, 0), 3.0)

    def insert_ingredient_orders(self):
        Countries.objects.create(
            id=0,
            name="country",
            country_code = "48"
        )
        Addresses.objects.create(
            id=0,
            country_id=0,
            postal_code="postal",
            city="city",
            street="street",
            street_number="number",
        )
        Suppliers.objects.create(id=0, name="name", bank_information="bank", address_id=0)

        for i in range(5):
            Ingredients.objects.create(id=i, name=str(i), is_price_per_kilogram=True, price=i + 1)

        for i in range(5):
            Ingredientssuppliers.objects.create(id=i, ingredient_id=i, supplier_id=0)

        dates = [datetime.datetime(2023, 5, 16, 9, 0), datetime.datetime(2023, 5, 16, 10, 0),
                 datetime.datetime(2023, 6, 10, 9, 0), datetime.datetime(2023, 6, 17, 9, 0),
                 datetime.datetime(2023, 6, 18, 9, 0) ]
        for i, date in enumerate(dates):
            Suppliersorders.objects.create(id=i, date=date, supplier_id=0)

        for i in range(5):
            Ingredientssuppliersorders.objects.create(id=i, ingredient_id=i, quantity=2, supplier_order_id=i)




    def setUp(self):
        self.insert_employee()
        self.insert_customer_orders()
        self.insert_ingredient_orders()


    def create_order(self, date, price):
        return Customersorders.objects.create(takeout=True, order_time=date, price=price, employee_id=0)

    def aggregated_asserts(self, data, expected):
        for granularity, expected in expected.items():
            aggregated = time_series_aggregated(granularity, data)
            for a, e in zip(aggregated, expected):
                assert a == e

    def test_customers_query(self):
        customers = customers_granularity("All")
        aggregated_expected = {
            'Day': [2, 1, 1, 1],
            'Week': [2, 1, 2],
            'Month': [2, 3],
            'Year': [5]
        }
        self.aggregated_asserts(customers, aggregated_expected)

    def test_expenses_query(self):
        expenses = expenses_granularity("All")
        aggregated_expected = {
            'Day': [6, 6, 8, 10],
            'Week': [6, 6, 18],
            'Month': [6, 24],
            'Year': [30]
        }
        self.aggregated_asserts(expenses, aggregated_expected)

    def test_income_query(self):
        income = income_granularity("All")
        aggregated_expected = {
            'Day': [3, 1, 2, 3],
            'Week': [3, 1, 5],
            'Month': [2, 6],
            'Year': [8]
        }
        self.aggregated_asserts(income, aggregated_expected)



class UpdatesTestCase(TestCase):
    def setUp(self):
        for i in range(1, 6):
            Offers.objects.create(name=str(i), price=i)


    def test_menu_insert(self):
        insert_menu_from_names("menu", ["1", "2", "5"])
        assert len(Menus.objects.all()) == 1

    def test_get_menu_dishes(self):
        dishes = ["1", "2", "5"]
        insert_menu_from_names("menu", dishes)
        assert set(get_menu_dishes("menu")) == set(dishes)

class ConstraintsTestCase(TestCase):
    def setUp(self):
        employee_type = Employeestypes.objects.create(name="0", min_salary=1000, max_salary=2000)
        self.employee_type_id = employee_type.id

        country = Countries.objects.create(
            name="country",
            country_code="48"
        )
        address = Addresses.objects.create(
            country_id=country.id,
            postal_code="postal",
            city="city",
            street="street",
            street_number="number",
        )

        self.supplier = Suppliers(name="a", bank_information="b", address_id=address.id)

        self.supplier_order = Suppliersorders.objects.create(
            date=datetime.date(2000, 1, 1),
            supplier_id=self.supplier.id
        )

        self.table = Tables.objects.create(
            seat_count=1,
            name="a",
            x_position=1,
            y_position=1
        )

    def test_constraint_salary(self):
        with self.assertRaises(IntegrityError):
            Employees.objects.create(
                name="a",
                surname="b",
                salary=0,
                birthdate=datetime.date(2000, 1, 1),
                employee_type_id=self.employee_type_id
            )

    def test_constraint_order_supplier(self):
        with self.assertRaises(IntegrityError):
            Ingredientssuppliersorders.objects.create(
                ingredient_id=0,
                supplier_order_id=self.supplier_order.id,
                quantity=10)

    def test_constraint_pesel(self):
        with self.assertRaises(IntegrityError):
            Employees.objects.create(
                name="a",
                surname="b",
                salary=1500,
                birthdate=datetime.date(2000, 1, 1),
                employee_type_id=self.employee_type_id,
                pesel="pesel"
            )

    def test_constraint_menu_overlap(self):
        with self.assertRaises(IntegrityError):
            Menus.objects.create(
                name="1",
                is_active=False,
                month_start=1,
                day_start=1,
                month_end=2,
                day_end=1
            )

            Menus.objects.create(
                name="1",
                is_active=False,
                month_start=1,
                day_start=10,
                month_end=2,
                day_end=1
            )

    def test_constraint_reservation_overlap(self):
        with self.assertRaises(IntegrityError):
            Reservations.objects.create(
                tables_id=self.table.id,
                start_time=datetime.datetime(year=2023, month=1, day=1, hour=10),
                end_time=datetime.datetime(year=2023, month=1, day=1, hour=15),
                name="1"
            )

            Reservations.objects.create(
                tables_id=self.table.id,
                start_time=datetime.datetime(year=2023, month=1, day=1, hour=11),
                end_time=datetime.datetime(year=2023, month=1, day=1, hour=14),
                name="2"
            )

    def test_constraint_invalid_offer(self):
        with self.assertRaises(IntegrityError):
            Offers.objects.create(
                name="1",
                type_id=1,
                price = 10.0,
                weight = 1.5,
            )

class DbConstraintTestCase(TestCase):
    def setUp(self) -> None:
        self.types = Employeestypes.objects.create(name="type", min_salary=1000, max_salary=2000)
        self.employee = Employees.objects.create(
            name="Name",
            surname="Name",
            salary=1500.0,
            birthdate=datetime.MINYEAR,
            pesel="1234",
            email="mail",
            phone_number="123456789",
            employee_type_id=0
        )
        self.offer_type = Offerstypes.objects.create("1")

    def test_order(self):
        with self.assertRaises(IntegrityError):
            Customersorders.objects.create(
                takeout=False,
                order_time=datetime.datetime(2022, 1, 1),
                finish_time=datetime.datetime(2021, 1, 1),
                employee_id=self.employee.id
            )

    def test_offer(self):
        with self.assertRaises(IntegrityError):
            Offers.objects.create(
                name = "1",
                type_id=self.offer_type.id,
                price = 1.0,
                weight = -1.0)

    def test_employee_type(self):
        with self.assertRaises(IntegrityError):
            Employeestypes.objects.create(name="a", min_salary=1000, max_salary=0)
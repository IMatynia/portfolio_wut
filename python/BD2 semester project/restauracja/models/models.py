# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#   * Rearrange models' order
#   * Make sure each model has one field with primary_key=True
#   * Make sure each ForeignKey and OneToOneField has `on_delete` set to the desired behavior
#   * Remove `managed = False` lines if you wish to allow Django to create, modify, and delete the table
# Feel free to rename the models, but don't rename db_table values or field names.
from django.db import models


class Addresses(models.Model):
    country = models.ForeignKey('Countries', models.DO_NOTHING)
    postal_code = models.CharField(max_length=10, blank=True, null=True)
    city = models.CharField(max_length=100, blank=True, null=True)
    street = models.CharField(max_length=100, blank=True, null=True)
    street_number = models.CharField(max_length=10, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'addresses'


class Allergies(models.Model):
    name = models.CharField(max_length=100)

    class Meta:
        managed = False
        db_table = 'allergies'


class Allergiesingredients(models.Model):
    allergy = models.ForeignKey(Allergies, models.DO_NOTHING)
    ingredient = models.ForeignKey('Ingredients', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'allergiesingredients'
        unique_together = (('allergy', 'ingredient'),)


class AuthGroup(models.Model):
    name = models.CharField(unique=True, max_length=150)

    class Meta:
        managed = False
        db_table = 'auth_group'


class AuthGroupPermissions(models.Model):
    id = models.BigAutoField(primary_key=True)
    group = models.ForeignKey(AuthGroup, models.DO_NOTHING)
    permission = models.ForeignKey('AuthPermission', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'auth_group_permissions'
        unique_together = (('group', 'permission'),)


class AuthPermission(models.Model):
    name = models.CharField(max_length=255)
    content_type = models.ForeignKey('DjangoContentType', models.DO_NOTHING)
    codename = models.CharField(max_length=100)

    class Meta:
        managed = False
        db_table = 'auth_permission'
        unique_together = (('content_type', 'codename'),)


class AuthUser(models.Model):
    password = models.CharField(max_length=128)
    last_login = models.DateTimeField(blank=True, null=True)
    is_superuser = models.BooleanField()
    username = models.CharField(unique=True, max_length=150)
    first_name = models.CharField(max_length=150)
    last_name = models.CharField(max_length=150)
    email = models.CharField(max_length=254)
    is_staff = models.BooleanField()
    is_active = models.BooleanField()
    date_joined = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'auth_user'


class AuthUserGroups(models.Model):
    id = models.BigAutoField(primary_key=True)
    user = models.ForeignKey(AuthUser, models.DO_NOTHING)
    group = models.ForeignKey(AuthGroup, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'auth_user_groups'
        unique_together = (('user', 'group'),)


class AuthUserUserPermissions(models.Model):
    id = models.BigAutoField(primary_key=True)
    user = models.ForeignKey(AuthUser, models.DO_NOTHING)
    permission = models.ForeignKey(AuthPermission, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'auth_user_user_permissions'
        unique_together = (('user', 'permission'),)


class Clients(models.Model):
    name = models.TextField()
    surname = models.TextField(blank=True, null=True)
    pronouns = models.TextField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'clients'


class Countries(models.Model):
    name = models.CharField(max_length=100)
    country_code = models.CharField(max_length=3, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'countries'


class Customersorders(models.Model):
    takeout = models.BooleanField()
    order_time = models.DateTimeField()
    finish_time = models.DateTimeField(blank=True, null=True)
    employee = models.ForeignKey('Employees', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'customersorders'


class Customersordersoffers(models.Model):
    customer_order = models.ForeignKey(Customersorders, models.DO_NOTHING)
    offer = models.ForeignKey('Offers', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'customersordersoffers'
        unique_together = (('customer_order', 'offer'),)


class Customersorderstables(models.Model):
    customer_order = models.ForeignKey(Customersorders, models.DO_NOTHING)
    table = models.ForeignKey('Tables', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'customersorderstables'
        unique_together = (('customer_order', 'table'),)


class DjangoAdminLog(models.Model):
    action_time = models.DateTimeField()
    object_id = models.TextField(blank=True, null=True)
    object_repr = models.CharField(max_length=200)
    action_flag = models.SmallIntegerField()
    change_message = models.TextField()
    content_type = models.ForeignKey('DjangoContentType', models.DO_NOTHING, blank=True, null=True)
    user = models.ForeignKey(AuthUser, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'django_admin_log'


class DjangoContentType(models.Model):
    app_label = models.CharField(max_length=100)
    model = models.CharField(max_length=100)

    class Meta:
        managed = False
        db_table = 'django_content_type'
        unique_together = (('app_label', 'model'),)


class DjangoMigrations(models.Model):
    id = models.BigAutoField(primary_key=True)
    app = models.CharField(max_length=255)
    name = models.CharField(max_length=255)
    applied = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'django_migrations'


class DjangoSession(models.Model):
    session_key = models.CharField(primary_key=True, max_length=40)
    session_data = models.TextField()
    expire_date = models.DateTimeField()

    class Meta:
        managed = False
        db_table = 'django_session'


class Employees(models.Model):
    name = models.CharField(max_length=100)
    surname = models.CharField(max_length=100)
    salary = models.DecimalField(max_digits=7, decimal_places=2)
    birthdate = models.DateTimeField()
    pesel = models.CharField(unique=True, max_length=11, blank=True, null=True)
    email = models.CharField(max_length=100, blank=True, null=True)
    phone_number = models.CharField(max_length=15, blank=True, null=True)
    employee_type = models.ForeignKey('Employeestypes', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'employees'


class Employeestypes(models.Model):
    name = models.CharField(max_length=100)
    min_salary = models.DecimalField(max_digits=7, decimal_places=2, blank=True, null=True)
    max_salary = models.DecimalField(max_digits=7, decimal_places=2, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'employeestypes'


class Ingredients(models.Model):
    name = models.CharField(max_length=100)
    is_price_per_kilogram = models.BooleanField()
    price = models.DecimalField(max_digits=6, decimal_places=2)

    class Meta:
        managed = False
        db_table = 'ingredients'


class Ingredientsoffers(models.Model):
    offer = models.ForeignKey('Offers', models.DO_NOTHING)
    ingredient = models.ForeignKey(Ingredients, models.DO_NOTHING)
    quantity = models.DecimalField(max_digits=7, decimal_places=4)

    class Meta:
        managed = False
        db_table = 'ingredientsoffers'
        unique_together = (('ingredient', 'offer'),)


class Ingredientssuppliers(models.Model):
    ingredient = models.ForeignKey(Ingredients, models.DO_NOTHING)
    supplier = models.ForeignKey('Suppliers', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'ingredientssuppliers'
        unique_together = (('ingredient', 'supplier'),)


class Ingredientssuppliersorders(models.Model):
    ingredient = models.ForeignKey(Ingredients, models.DO_NOTHING)
    supplier_order = models.ForeignKey('Suppliersorders', models.DO_NOTHING)
    quantity = models.IntegerField()

    class Meta:
        managed = False
        db_table = 'ingredientssuppliersorders'
        unique_together = (('ingredient', 'supplier_order'),)


class Menus(models.Model):
    name = models.CharField()
    month_start = models.IntegerField()
    day_start = models.IntegerField()
    month_end = models.IntegerField()
    day_end = models.IntegerField()

    class Meta:
        managed = False
        db_table = 'menus'


class Menusoffers(models.Model):
    menu = models.ForeignKey(Menus, models.DO_NOTHING)
    offer = models.ForeignKey('Offers', models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'menusoffers'
        unique_together = (('menu', 'offer'),)


class Offers(models.Model):
    name = models.CharField(max_length=100)
    type = models.ForeignKey('Offerstypes', models.DO_NOTHING)
    price = models.DecimalField(max_digits=6, decimal_places=2)
    weight = models.DecimalField(max_digits=6, decimal_places=4, blank=True, null=True)
    volume = models.DecimalField(max_digits=6, decimal_places=4, blank=True, null=True)
    alcohol_by_volume = models.DecimalField(max_digits=5, decimal_places=2, blank=True, null=True)
    is_served_cold = models.BooleanField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'offers'


class Offerstypes(models.Model):
    name = models.CharField(max_length=100, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'offerstypes'


class Reservations(models.Model):
    tables = models.ForeignKey('Tables', models.DO_NOTHING)
    start_time = models.DateTimeField()
    end_time = models.DateTimeField()
    name = models.CharField(max_length=100, blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'reservations'


class Suppliers(models.Model):
    name = models.CharField(max_length=100, blank=True, null=True)
    bank_information = models.CharField(max_length=100, blank=True, null=True)
    address = models.ForeignKey(Addresses, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'suppliers'


class Suppliersorders(models.Model):
    date = models.DateTimeField()
    supplier = models.ForeignKey(Suppliers, models.DO_NOTHING)

    class Meta:
        managed = False
        db_table = 'suppliersorders'


class Tables(models.Model):
    seat_count = models.IntegerField()
    name = models.CharField(max_length=100, blank=True, null=True)
    x_position = models.IntegerField()
    y_position = models.IntegerField()

    class Meta:
        managed = False
        db_table = 'tables'

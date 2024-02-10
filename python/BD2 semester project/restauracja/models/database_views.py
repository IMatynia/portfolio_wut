from django.db import models

class Customerordersfull(models.Model):
    takeout = models.BooleanField()
    order_time = models.DateTimeField()
    finish_time = models.DateTimeField(blank=True, null=True)
    employee = models.ForeignKey('Employees', models.DO_NOTHING)
    name = models.TextField()
    surname = models.TextField()
    is_finished = models.BooleanField()
    total_cost = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'customers_orders_detailed'
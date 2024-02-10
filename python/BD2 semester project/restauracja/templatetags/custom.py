from django import template
import datetime

register = template.Library()


@register.filter
def monthtextual(value):
    return datetime.date(2020, value, 1).strftime('%B')

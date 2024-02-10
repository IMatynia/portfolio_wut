from django.urls import path
from django.shortcuts import redirect
from .views import *

urlpatterns = [
    path("", WelcomePage.as_view()),
    path("datavis", PlottingView.as_view()),
    path("tablemanager", TableManager.as_view()),
    path("api/<str:endpoint>/", ApiView.as_view()),
    path("menu", MenuShowActive.as_view()),
    path("menu/<int:menu_id>/", MenuShow.as_view()),
    path("menumanager", MenuManager.MenusIndex.as_view()),
    path("menumanager/edit/<int:menu_id>/", MenuManager.EditMenu.as_view()),
    path("orderbacklog", OrderBacklog.as_view()),
    path("test", TestView.as_view()),
]

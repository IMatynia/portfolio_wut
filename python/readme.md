# PAP - Programowanie aplikacyjne / Application programming
> 2022Z

Technologies:
- Python
- Oracle DB
- PySide (Qt)

Co-authors: Bartosz Han, Mykhailo Marfenko

In case of this semester project, we could choose the theme. In a team of 3, we have decided to create an app that is a warsaw public transport utility for a passenger, based on real world data stored on our project's database. It allows for the displaying of time tables, bus/tram/train routes on a map, bus/tram/train stop complexes (when there are multiple stops with the same name, but different number and location, also displayed on a map) and navigation. More information is available in "dokumentacja.pdf"

Examples of my work:
- stop complex map (`src/controllers/stop_layout.py`)
- entirety of A* navigation algorithm (`src/lib/*`)
- space-time chunking of the network graph for query optimization (`src/models/nav_*`)
- some utilities such as the user config, MVC project structure or location picker (`src/models/user_config_model.py` `src/controllers/pick_location_widget.py`)

All of the code is written in python. The UI was made using PySide6 (Qt for python).

# BD2 - bazy danych 2 / databases 2
> 2022L

Technologies:
- Python
- Django
- PostgreSQL
- Plotly
- Jinja
- Bootstrap5

Co-authors: Bartosz Han, Marcin Łojek, Jan Fidor

In this project we were tasked with creating a restaurant managing web service. I was tasked with working on the front-end (bootstrap, jinja, plotly) as well as most of the backend (django).

# PIPR - podstawy informatyki i programowania / computer science basics
> 2021Z

Technologies:
- Python
- Pyside (Qt)

Shannon Switching game Gale variant by Igor Matynia. More detailed information is available in "dokumentacja.pdf"

First individual semester project. All code written in python. The UI was made using PySide2.

# POP - przeszukiwanie i optymalizacja / search and optimization algorithms
> 2023Z

Technologies:
- Python
- numpy

Co-author: Andrii Gamalii

For this project we were supposed to come up with a heuristic for solving the traveling salesman problem, with additional limitations described here: https://www.kaggle.com/c/traveling-santa-2018-prime-paths

# UMA - Uczenie maszynowe / machine learning
> 2023Z

Technolgies:
- python
- numpy

Co-author: Filip Stefański

For this project we had to implement a differential evolution algorith with hyperparameter optimization using reinforcement learning, which in our case was QLearning. I was responsible for the implementation of Qlearning, training scripts and graphing scripts.

# PSI - programowanie sieciowe / network programming
> 2023Z

Technologies:
- python
- openssl
- docker

Co-authors: Bartłomiej Pełka, Andrii Gamali, Wiktor Topolski

In this project we were tasked with creating an anonimizing network system akin to a simplified TOR. I coded the API and the recipient. 


# WSI - wstęp do sztucznej inteligencji / introduction to artificial intelligence
> 2022Z

Technologies:
- python
- numpy
- pandas

Co-author: Bartosz Han

In each subfolder there is code for the specified algorithm, such as ID3, minmax, qlearning or an ANN.
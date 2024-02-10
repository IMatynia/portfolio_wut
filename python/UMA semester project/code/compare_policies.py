"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""


from src.training.trainer import Trainer

if __name__ == "__main__":
    t = Trainer()
    t.init()
    t.run_policy()

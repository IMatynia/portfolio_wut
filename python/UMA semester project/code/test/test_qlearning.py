"""
Projekt UMA 23Z
Autorzy:
Igor Matynia
"""

from src.functions.function import Function
from src.qlearning.qlearning import Qlearning
from src.qlearning.qlearningenvironment import QLearningEnvironment
from src.qlearning.lineardiscretisation import StateSpaceLinearDescretisation, ActionSpaceLiearDescretisation


def test_linear_descritization():
    state_space = StateSpaceLinearDescretisation(10.0, 4, 3, 10)
    action_space = ActionSpaceLiearDescretisation(["A", "B", "C", "D"], [0.5, 0.75, 1.0, 2.0])

    assert state_space.get_shape() == 4 * 3
    assert state_space.get_state_representation(0, 0) == 0
    assert state_space.get_state_representation(10000, 0.5) == 10
    assert state_space.get_state_representation(3, 0.8) == 2

    assert action_space.get_shape() == 4 * 4
    assert action_space.get_action_from_representation(0) == (0.5, "A")
    assert action_space.get_action_from_representation(2 * 4 + 3) == (2.0, "C")


def test_qlearning_init():
    state_space = StateSpaceLinearDescretisation(10.0, 4, 3, 10)
    action_space = ActionSpaceLiearDescretisation(["A", "B", "C", "D"], [0.5, 0.75, 1.0, 2.0])
    env = QLearningEnvironment(state_space, action_space, Function(1, 10, -1, 1), 100)
    qlearning = Qlearning(
        0.1,
        0.2,
        0.2,
        state_space,
        action_space,
        1000
    )

    assert qlearning.policy.shape == (4 * 3, 4 * 4)


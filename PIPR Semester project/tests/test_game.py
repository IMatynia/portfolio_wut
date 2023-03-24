from game_code.game import (GameConfig, GaleShannonGame,
                            OpponentNotAgentError, InvalidMoveError)
from game_code.ai import SwitchBot, Agent
from game_code.constants import (
    OPPONENT_SIDE,
    PLAYER_SIDE,
    VERTICAL,
    HORIZONTAL,
)
import pytest


def test_game_config_init():
    n = 5
    opponent = SwitchBot(0.5)
    config = GameConfig(n, opponent, PLAYER_SIDE)

    assert config.n == n
    assert config.opponent == opponent
    assert config.who_starts == PLAYER_SIDE
    assert config.connections is None
    assert config.opponents_last_move is None
    assert config.players_last_move is None


def test_game_init_fresh():
    n = 5
    opponent = SwitchBot(0.5)
    config = GameConfig(
        n,
        opponent,
        PLAYER_SIDE)

    game = GaleShannonGame(config)
    game.get_current_side() == PLAYER_SIDE
    game._opponent == opponent
    game.get_board().get_n() == n


def test_game_init_with_connections():
    n = 5
    opponent = SwitchBot(0.5)
    connections = [
        {
            "x": 0,
            "y": 0,
            "direction": VERTICAL
        },
        {
            "x": 1,
            "y": 1,
            "direction": HORIZONTAL
        },
        {
            "x": 2,
            "y": 0,
            "direction": HORIZONTAL
        }
    ]
    config = GameConfig(
        n,
        opponent,
        OPPONENT_SIDE,
        connections,
        (0, 0),
        (1, 1)
    )

    game = GaleShannonGame(config)

    assert game.get_current_side() == OPPONENT_SIDE
    brd = game.get_board()
    assert brd.get_n() == n
    conn_set = set([(0, 0), (1, 1), (2, 0)])
    assert brd.get_available_connectors().intersection(conn_set) == set([])
    assert brd.get_board_field(0, 0).get_direction() == VERTICAL
    assert brd.get_board_field(1, 1).get_direction() == HORIZONTAL
    assert brd.get_board_field(2, 0).get_direction() == HORIZONTAL
    assert brd.UP.get_id() == brd.get_master_id_of_static(
        brd.get_board_field(0, 1))
    assert brd.UP.get_id() == brd.get_master_id_of_static(
        brd.get_board_field(2, 1))
    assert brd.get_master_id_of_static(brd.get_board_field(
        1, 0)) == brd.get_master_id_of_static(brd.get_board_field(3, 0))
    assert game.get_opponent_last_move() == (0, 0)
    assert game.get_players_last_move() == (1, 1)


def test_game_play_no_agent():
    # gaming with a fella
    config = GameConfig(
        5,
        None,
        PLAYER_SIDE)
    game = GaleShannonGame(config)
    game.play_round(PLAYER_SIDE, (0, 0))
    assert game.get_board().get_board_field(0, 0).get_side() == PLAYER_SIDE
    game.play_round(PLAYER_SIDE, (2, 0))
    assert game.get_board().get_board_field(2, 0).get_side() == PLAYER_SIDE
    game.play_round(OPPONENT_SIDE, (1, 1))
    assert game.get_board().get_board_field(1, 1).get_side() == OPPONENT_SIDE
    game.play_round(OPPONENT_SIDE, (1, 3))
    assert game.get_board().get_board_field(1, 3).get_side() == OPPONENT_SIDE
    game.play_round(OPPONENT_SIDE, (2, 2))
    assert game.get_board().get_board_field(2, 2).get_side() == OPPONENT_SIDE
    game.play_round(PLAYER_SIDE, (4, 0))
    assert game.get_board().get_board_field(4, 0).get_side() == PLAYER_SIDE

    assert game.check_for_win() == PLAYER_SIDE


def test_game_side_switching():
    config = GameConfig(
        5,
        None,
        OPPONENT_SIDE)
    game = GaleShannonGame(config)
    assert game.get_current_side() == OPPONENT_SIDE
    game.play_round(game.get_current_side(), (0, 0))
    assert game.get_current_side() == PLAYER_SIDE
    game.play_round(game.get_current_side(), (1, 1))
    assert game.get_current_side() == OPPONENT_SIDE


def test_game_playtest_exceptions():
    config = GameConfig(
        5,
        None,
        PLAYER_SIDE)
    game = GaleShannonGame(config)

    with pytest.raises(OpponentNotAgentError):
        game.play_round(OPPONENT_SIDE)

    with pytest.raises(InvalidMoveError):
        game.play_round(PLAYER_SIDE)


def test_game_playtest_with_dummy_agent(monkeypatch):
    config = GameConfig(
        5,
        Agent(),
        OPPONENT_SIDE)
    game = GaleShannonGame(config)

    opp_moves = [
        (1, 1),
        (3, 1),
    ]

    def move_generator():
        mv = opp_moves[0]
        del opp_moves[0]
        return mv

    monkeypatch.setattr(game._opponent, "get_next_move", move_generator)

    assert game.get_current_side() == OPPONENT_SIDE
    game.play_round(game.get_current_side())
    assert game.get_board().get_board_field(1, 1).get_side() == OPPONENT_SIDE

    assert game.get_current_side() == PLAYER_SIDE
    game.play_round(game.get_current_side(), (0, 0))
    assert game.get_board().get_board_field(0, 0).get_side() == PLAYER_SIDE

    assert game.get_current_side() == OPPONENT_SIDE
    game.play_round(game.get_current_side())
    assert game.get_board().get_board_field(3, 1).get_side() == OPPONENT_SIDE

from game_code.ai import (
    NoBoardReferenceError,
    InvalidFractionError,
    ExtendedMove,
    DumBot,
    HeuriBot,
    SwitchBot
)
from game_code.board import Board
from game_code.game_data_io import read_game_config_from_json_stream
from game_code.constants import (
    PLAYER_SIDE,
    OPPONENT_SIDE
)
from io import StringIO
from pytest import approx, raises


def test_extended_move_inits():
    move1 = ExtendedMove((0, 0))
    assert move1.get_move() == (0, 0)

    move2 = ExtendedMove((1, 1), 15, "text")
    assert move2.get_move() == (1, 1)
    assert move2.get_info() == "text\n"
    assert move2.get_score() == 15


def test_extended_move_modifying_data():
    move = ExtendedMove((0, 0))
    move.add_score(10)
    move.add_info_line("This move is very strategic you see")
    assert move.get_score() == 10
    assert move.get_info() == "\nThis move is very strategic you see\n"


def test_extended_move_formatted_info():
    move = ExtendedMove((1, 1), 15, "{score}, {x}, {y}")
    assert move.get_info() == "15, 1, 1\n"
    move.add_score(100)
    move.add_info_line("Hello :)")
    assert move.get_info() == "115, 1, 1\nHello :)\n"


def test_ai_agents_inits():
    n = 7
    board = Board(n)

    dumbot = DumBot()
    heuribot = HeuriBot()
    fraction = 0.5
    switchbot = SwitchBot(fraction)

    dumbot.set_board_ref(board)
    heuribot.set_board_ref(board)
    switchbot.set_board_ref(board)

    assert dumbot.get_board() == board
    assert heuribot.get_board() == board
    assert switchbot.get_board() == board

    assert switchbot.get_fraction() == fraction


def test_ai_agents_no_brd_reference_error():
    dumbot = DumBot()
    heuribot = HeuriBot()
    fraction = 0.5
    switchbot = SwitchBot(fraction)

    with raises(NoBoardReferenceError):
        dumbot.get_next_move()
    with raises(NoBoardReferenceError):
        heuribot.get_next_move()
    with raises(NoBoardReferenceError):
        switchbot.get_next_move()

    assert switchbot.get_fraction() == fraction


def test_ai_dumbot_get_move(monkeypatch):
    n = 7
    brd = Board(n)
    dumbot = DumBot(brd)

    def fake_rand_choice(vals):
        return list(vals)[0]

    monkeypatch.setattr("game_code.ai.random.choice", fake_rand_choice)

    move = dumbot.get_next_move()
    assert move.get_move() == list(brd.get_available_connectors())[0]
    assert move.get_info() == "Dumbots guess @(4,0)\n"


def test_ai_heuribot_calculate_move_score_examples():
    # Reading a board from a saved game
    stream = StringIO(
        "{\"connections\": [{\"x\": 6, \"y\": 0, \"direction\": -1}, {\"x\": 0, \"y\": 2, \"direction\": 1}, {\"x\": 2, \"y\": 2, \"direction\": 1}, {\"x\": 6, \"y\": 2, \"direction\": -1}, {\"x\": 5, \"y\": 3, \"direction\": -1}, {\"x\": 2, \"y\": 4, \"direction\": 1}, {\"x\": 6, \"y\": 4, \"direction\": -1}, {\"x\": 1, \"y\": 7, \"direction\": -1}, {\"x\": 6, \"y\": 8, \"direction\": -1}], \"n\": 9, \"opponents_last_move\": [6, 8], \"players_last_move\": [5, 3], \"who_starts\": -1, \"opponent_type\": \"Player\"}"
    )

    config = read_game_config_from_json_stream(stream)
    brd = Board(config.n)
    brd.load_state(config.connections)

    heuribot = HeuriBot(brd)
    assert approx(heuribot.calculate_move(4, 2, PLAYER_SIDE).get_score(),
                  abs=0.001) == 31.1111
    assert approx(heuribot.calculate_move(6, 6, OPPONENT_SIDE).get_score(),
                  abs=0.001) == 1e+55
    assert approx(heuribot.calculate_move(1, 5, PLAYER_SIDE).get_score(),
                  abs=0.001) == 10.0
    assert approx(heuribot.calculate_move(3, 3, PLAYER_SIDE).get_score(),
                  abs=0.001) == 20.0


def test_ai_heuribot_calculate_total_score_examples():
    # Reading a board from a saved game
    stream = StringIO(
        "{\"connections\": [{\"x\": 6, \"y\": 0, \"direction\": -1}, {\"x\": 0, \"y\": 2, \"direction\": 1}, {\"x\": 2, \"y\": 2, \"direction\": 1}, {\"x\": 6, \"y\": 2, \"direction\": -1}, {\"x\": 5, \"y\": 3, \"direction\": -1}, {\"x\": 2, \"y\": 4, \"direction\": 1}, {\"x\": 6, \"y\": 4, \"direction\": -1}, {\"x\": 1, \"y\": 7, \"direction\": -1}, {\"x\": 6, \"y\": 8, \"direction\": -1}], \"n\": 9, \"opponents_last_move\": [6, 8], \"players_last_move\": [5, 3], \"who_starts\": -1, \"opponent_type\": \"Player\"}"
    )

    config = read_game_config_from_json_stream(stream)
    brd = Board(config.n)
    brd.load_state(config.connections)

    heuribot = HeuriBot(brd)
    assert approx(heuribot.calculate_total_score_of_pos(4, 2).get_score(),
                  abs=0.001) == 51.1111
    assert approx(heuribot.calculate_total_score_of_pos(6, 6).get_score(),
                  abs=0.001) == 1.2e+55
    assert approx(heuribot.calculate_total_score_of_pos(1, 5).get_score(),
                  abs=0.001) == 22.0
    assert approx(heuribot.calculate_total_score_of_pos(3, 3).get_score(),
                  abs=0.001) == 32.0


def test_ai_heuribot_get_next_move_examples():
    def check_this_board(config):
        stream = StringIO(config)

        config = read_game_config_from_json_stream(stream)
        brd = Board(config.n)
        brd.load_state(config.connections)

        heuribot = HeuriBot(brd)
        return heuribot.get_next_move().get_move()

    # Winning move
    assert check_this_board(
        "{\"connections\": [{\"x\": 6, \"y\": 0, \"direction\": -1}, {\"x\": 0, \"y\": 2, \"direction\": 1}, {\"x\": 2, \"y\": 2, \"direction\": 1}, {\"x\": 6, \"y\": 2, \"direction\": -1}, {\"x\": 5, \"y\": 3, \"direction\": -1}, {\"x\": 2, \"y\": 4, \"direction\": 1}, {\"x\": 6, \"y\": 4, \"direction\": -1}, {\"x\": 1, \"y\": 7, \"direction\": -1}, {\"x\": 6, \"y\": 8, \"direction\": -1}], \"n\": 9, \"opponents_last_move\": [6, 8], \"players_last_move\": [5, 3], \"who_starts\": -1, \"opponent_type\": \"Player\"}"
    ) == (6, 6)

    # Arrow
    assert check_this_board(
        '{"connections": [{"x": 2, "y": 0, "direction": 1}, {"x": 1, "y": 1, "direction": -1}, {"x": 3, "y": 1, "direction": 1}, {"x": 2, "y": 2, "direction": -1}], "n": 7, "opponents_last_move": [2, 2], "players_last_move": [1, 1], "who_starts": 1, "opponent_type": "Player"}'
    ) == (0, 0)

    # eg.game1
    assert check_this_board(
        '{"connections": [{"x": 2, "y": 0, "direction": -1}, {"x": 0, "y": 2, "direction": 1}, {"x": 2, "y": 2, "direction": -1}, {"x": 3, "y": 3, "direction": -1}, {"x": 2, "y": 4, "direction": 1}], "n": 5, "opponents_last_move": [2, 0], "players_last_move": [3, 3], "who_starts": -1, "opponent_type": "Player"}'
    ) == (0, 4)

    # eg.game2
    assert check_this_board(
        '{"connections": [{"x": 2, "y": 2, "direction": -1}, {"x": 4, "y": 2, "direction": 1}, {"x": 3, "y": 3, "direction": -1}, {"x": 2, "y": 4, "direction": 1}, {"x": 4, "y": 4, "direction": -1}], "n": 7, "opponents_last_move": [4, 4], "players_last_move": [2, 4], "who_starts": -1, "opponent_type": "Player"}'
    ) == (0, 0)


def test_ai_switch_bot_0_5_frac(monkeypatch):
    choices = [
        0.1,
        0.9,
        0.5,
        1.0,
        0.0
    ]

    fake_heuri_decition = ExtendedMove((0, 0))
    fake_dumb_decition = ExtendedMove((1, 1))

    def fake_rand():
        return choices.pop()

    def fake_get_move_heuri():
        return fake_heuri_decition

    def fake_get_move_dumbot():
        return fake_dumb_decition

    brd = Board(5)
    switchbot = SwitchBot(0.5, brd)
    monkeypatch.setattr(
        "game_code.ai.random.random", fake_rand)
    monkeypatch.setattr(
        switchbot._dumbot, "get_next_move", fake_get_move_dumbot)
    monkeypatch.setattr(
        switchbot._heuribot, "get_next_move", fake_get_move_heuri)

    assert switchbot.get_next_move() == fake_heuri_decition
    assert switchbot.get_next_move() == fake_dumb_decition
    assert switchbot.get_next_move() == fake_dumb_decition
    assert switchbot.get_next_move() == fake_dumb_decition
    assert switchbot.get_next_move() == fake_heuri_decition


def test_ai_switch_bot_1_0_frac(monkeypatch):
    choices = [
        0.1,
        0.9,
        0.5,
        1.0,
        0.0
    ]

    fake_heuri_decition = ExtendedMove((0, 0))
    fake_dumb_decition = ExtendedMove((1, 1))

    def fake_rand():
        return choices.pop()

    def fake_get_move_heuri():
        return fake_heuri_decition

    def fake_get_move_dumbot():
        return fake_dumb_decition

    brd = Board(5)
    switchbot = SwitchBot(1.0, brd)
    monkeypatch.setattr(
        "game_code.ai.random.random", fake_rand)
    monkeypatch.setattr(
        switchbot._dumbot, "get_next_move", fake_get_move_dumbot)
    monkeypatch.setattr(
        switchbot._heuribot, "get_next_move", fake_get_move_heuri)

    assert switchbot.get_next_move() == fake_heuri_decition
    assert switchbot.get_next_move() == fake_heuri_decition
    assert switchbot.get_next_move() == fake_heuri_decition
    assert switchbot.get_next_move() == fake_heuri_decition
    assert switchbot.get_next_move() == fake_heuri_decition


def test_ai_switch_bot_0_0_frac(monkeypatch):
    choices = [
        0.1,
        0.9,
        0.5,
        1.0,
        0.0
    ]

    fake_heuri_decition = ExtendedMove((0, 0))
    fake_dumb_decition = ExtendedMove((1, 1))

    def fake_rand():
        return choices.pop()

    def fake_get_move_heuri():
        return fake_heuri_decition

    def fake_get_move_dumbot():
        return fake_dumb_decition

    brd = Board(5)
    switchbot = SwitchBot(0.0, brd)
    monkeypatch.setattr(
        "game_code.ai.random.random", fake_rand)
    monkeypatch.setattr(
        switchbot._dumbot, "get_next_move", fake_get_move_dumbot)
    monkeypatch.setattr(
        switchbot._heuribot, "get_next_move", fake_get_move_heuri)

    assert switchbot.get_next_move() == fake_dumb_decition
    assert switchbot.get_next_move() == fake_dumb_decition
    assert switchbot.get_next_move() == fake_dumb_decition
    assert switchbot.get_next_move() == fake_dumb_decition
    assert switchbot.get_next_move() == fake_dumb_decition


def test_ai_switch_bot_invalid_fraction():
    with raises(InvalidFractionError):
        SwitchBot(1312321312)

    with raises(InvalidFractionError):
        SwitchBot(-0.1)

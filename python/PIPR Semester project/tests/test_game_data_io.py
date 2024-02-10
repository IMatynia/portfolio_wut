from game_code.game_data_io import (
    read_game_config_from_json_stream,
    write_game_config_to_stream
)
from io import StringIO
from game_code.game import GameConfig
from game_code.ai import SwitchBot
from game_code.constants import (
    OPPONENT_SIDE,
    PLAYER_SIDE,
    VERTICAL,
    HORIZONTAL
)


def test_simple_read_write_config():
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

    stream = StringIO()
    write_game_config_to_stream(stream, config)
    stream.seek(0)

    config2 = read_game_config_from_json_stream(stream)
    assert config.n == config2.n
    assert config.connections == config2.connections
    assert config.who_starts == config2.who_starts
    assert config.players_last_move == config2.players_last_move
    assert config.opponents_last_move == config2.opponents_last_move


def test_read_config():
    stream = StringIO("{\"connections\": [{\"x\": 6, \"y\": 0, \"direction\": -1}, {\"x\": 0, \"y\": 2, \"direction\": 1}, {\"x\": 2, \"y\": 2, \"direction\": 1}, {\"x\": 6, \"y\": 2, \"direction\": -1}, {\"x\": 5, \"y\": 3, \"direction\": -1}, {\"x\": 2, \"y\": 4, \"direction\": 1}, {\"x\": 6, \"y\": 4, \"direction\": -1}, {\"x\": 1, \"y\": 7, \"direction\": -1}, {\"x\": 6, \"y\": 8, \"direction\": -1}], \"n\": 9, \"opponents_last_move\": [6, 8], \"players_last_move\": [5, 3], \"who_starts\": -1, \"opponent_type\": \"Player\"}")

    config = read_game_config_from_json_stream(stream)

    assert config.connections == [
        {'x': 6, 'y': 0, 'direction': -1},
        {'x': 0, 'y': 2, 'direction': 1},
        {'x': 2, 'y': 2, 'direction': 1},
        {'x': 6, 'y': 2, 'direction': -1},
        {'x': 5, 'y': 3, 'direction': -1},
        {'x': 2, 'y': 4, 'direction': 1},
        {'x': 6, 'y': 4, 'direction': -1},
        {'x': 1, 'y': 7, 'direction': -1},
        {'x': 6, 'y': 8, 'direction': -1}
    ]
    assert config.n == 9
    assert config.opponent is None
    assert config.opponents_last_move == (6, 8)
    assert config.players_last_move == (5, 3)
    assert config.who_starts == OPPONENT_SIDE


def test_write_config():
    stream = StringIO()

    config = GameConfig(7, SwitchBot(0.69), PLAYER_SIDE)
    write_game_config_to_stream(stream, config)

    stream.seek(0)
    assert stream.read(
    ) == '{"connections": null, "n": 7, "opponents_last_move": null, "players_last_move": null, "who_starts": 1, "opponent_type": "SwitchBot", "fraction": 0.69}'

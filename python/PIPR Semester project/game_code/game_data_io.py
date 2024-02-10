import json
from game_code.game import GameConfig
from game_code.ai import SwitchBot


def read_game_config_from_json_stream(stream):
    """Reads the stream and parses it into a GameConfig

    Args:
        stream (Stream): the json text stream

    Returns:
        GameConfig: the parsed config
    """
    json_data = json.load(stream)
    config = GameConfig(None, None, None)

    # Reading data from json into GameConfig object
    config.connections = list(json_data["connections"])
    config.n = int(json_data["n"])
    config.opponents_last_move = tuple(json_data["opponents_last_move"])
    config.players_last_move = tuple(json_data["players_last_move"])
    config.who_starts = int(json_data["who_starts"])

    # Creating the opponent instance
    opponent_type = str(json_data["opponent_type"])
    if opponent_type == "SwitchBot":
        fraction = float(json_data["fraction"])
        config.opponent = SwitchBot(fraction)
    elif opponent_type == "Player":
        config.opponent = None

    return config


def write_game_config_to_stream(stream, config):
    """Writes the GameConfig to the stream.

    Args:
        stream (Stream): the output data stream
        config (GameConfig): the config to be parsed
    """
    json_data = {}

    # Saving GameCofnig data
    json_data["connections"] = config.connections
    json_data["n"] = config.n
    json_data["opponents_last_move"] = config.opponents_last_move
    json_data["players_last_move"] = config.players_last_move
    json_data["who_starts"] = config.who_starts

    # Creating the opponent instance
    if config.opponent:
        json_data["opponent_type"] = "SwitchBot"
        json_data["fraction"] = config.opponent.get_fraction()
    else:
        json_data["opponent_type"] = "Player"

    json.dump(json_data, stream)

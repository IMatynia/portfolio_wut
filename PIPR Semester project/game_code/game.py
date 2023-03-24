from game_code.board import (
    Board, get_direction, InvalidSideError
)
from game_code.constants import (
    OPPONENT_SIDE,
    PLAYER_SIDE,
    VALID_SIDES)
from game_code.ai import Agent, ExtendedMove
import logging as log


class OpponentNotAgentError(Exception):
    pass


class InvalidMoveError(Exception):
    pass


class GameConfig:
    def __init__(self,
                 n,
                 opponent,
                 who_starts,
                 connections=None,
                 opponents_last_move=None,
                 players_last_move=None):
        """A data class storing info about the configuration of the game

        Args:
            n (int): boards size
            opponent (Agent): Decision making agent
            who_starts (int): Side code of the player who starts
            connections (list, optional): list of dictionaries
                of saved connections. Defaults to None.
            opponents_last_move (tuple, optional): coordinate tuple.
                Defaults to None.
            players_last_move (tuple, optional): coordinate tuple.
                Defaults to None.
        """
        self.n = n
        self.opponent = opponent
        self.who_starts = who_starts
        self.connections = connections
        self.opponents_last_move = opponents_last_move
        self.players_last_move = players_last_move


class GaleShannonGame:
    """
    Game class. Contains all methods needed to play the game.
    """

    def __init__(self, game_config):
        """Creates a game instance

        Args:
            game_config (GameConfig): contains all information needed to set
                up the game
        """
        self._config = game_config
        self._board = Board(game_config.n)
        self._opponent = game_config.opponent
        self._current_side = game_config.who_starts

        self._opponents_last_move = game_config.opponents_last_move
        self._players_last_move = game_config.players_last_move

        if self._opponent:
            self._opponent.set_board_ref(self._board)

        if game_config.connections:
            self._board.load_state(game_config.connections)

    def play_round(self, side, move=None):
        """Plays a round of the game

        Args:
            side (int): side code
            move (tuple(int, int), optional): Location of the move. If None
                and opponent side, agent will try generating it.
                Defaults to None.

        Raises:
            OpponentNotAgentError: If move requested but agent is incapable of
                computing it
            InvalidMoveError: If the move is needed but invalid data is given
        """
        # Getting the move and exceptions
        if move is None and side == OPPONENT_SIDE:
            if self.opponent_is_agent():
                move = self._opponent.get_next_move()
            else:
                raise OpponentNotAgentError()
        elif move is None and side == PLAYER_SIDE:
            raise InvalidMoveError()

        if type(move) == ExtendedMove:
            log.info(move)
            move = move.get_move()

        x, y = move
        # Choose direction
        direction = get_direction(x, y, side)

        # Place connection
        self._board.place_connection(x, y, direction)

        # Save last move
        if side == OPPONENT_SIDE:
            self._opponents_last_move = move
        else:
            self._players_last_move = move

        # Save the current side
        self.set_current_side(-side)

    def check_for_win(self):
        """Checks if any side won the game, returns code NO_SIDE if there
        is no winner yet

        Returns:
            int: side code of the side that won
        """
        return self._board.check_win()

    def opponent_is_agent(self):
        """Is the opponent an Agent, or is he a None? That is the question

        Returns:
            bool: Logical result of the query
        """
        return isinstance(self._opponent, Agent)

    def get_board(self):
        """Returns a reference to the game board

        Returns:
            Board: the board
        """
        return self._board

    def set_current_side(self, side):
        """Sets the current side that is meant to play in the next round

        Args:
            side (int): side code

        Raises:
            InvalidSideError: if the side given is invalid
        """
        if side in VALID_SIDES:
            self._current_side = side
        else:
            raise InvalidSideError(side)

    def get_current_side(self):
        """Returns the side that is meant to play now

        Returns:
            int: side code
        """
        return self._current_side

    def get_opponent_last_move(self):
        """Returns opponents last move coordinates tuple

        Returns:
            tuple: (x, y) coordinates
        """
        return self._opponents_last_move

    def get_players_last_move(self):
        """Returns players last move coordinates tuple

        Returns:
            tuple: (x, y) coordinates
        """
        return self._players_last_move

    def get_sides_nickname(self):
        """Returns the nickname of the side whoose turn it is in the game

        Returns:
            str: the nickname
        """
        if self._current_side == PLAYER_SIDE:
            return "You"
        else:
            return "Opp"

    def create_config(self):
        """Creates a configuration object based on the
        current state of the game

        Returns:
            GameConfig: generated config object
        """
        config = GameConfig(
            n=self._board.get_n(),
            opponent=self._opponent,
            who_starts=self._current_side,
            connections=self._board.dump_state(),
            opponents_last_move=self._opponents_last_move,
            players_last_move=self._players_last_move
        )
        return config

    def info(self):
        """Returns a description of the game object

        Returns:
            str: the info
        """
        table = []
        for y in range(self._config.n):
            row = []
            for x in range(self._config.n):
                row.append(self._board.get_board_field(x, y).what_to_display())
            table.append(row)
        table_str = str(table)
        return f"State of the board:\n{table_str}\n"

    def __str__(self):
        return self.info()

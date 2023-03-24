import random
from game_code.constants import (
    OPPONENT_SIDE,
    PLAYER_SIDE
)
from game_code.board import get_direction


class NoBoardReferenceError(Exception):
    pass


class InvalidFractionError(Exception):
    def __init__(self, fraction):
        super().__init__(f"{fraction} is an invalid fraction!")
        self.fraction = fraction


class ExtendedMove:
    def __init__(self, move, score=0, info=""):
        """Creates extended move object. Used to store information about a move.
        Debug info string has access to x, y, and score as
        via string formatting.

        Args:
            move (tuple): x, y coordinates of the move
            score (int, optional): The score of the move. Defaults to 0.
            info (str, optional): Additional information used for debugging.
                Defaults to "".
        """
        self._move = move
        self._info = info
        self._score = score

    def add_info_line(self, line):
        """Adds a line of text to the debug info

        Args:
            line (str): the additional text
        """
        self._info += "\n" + line

    def add_score(self, points):
        """Adds points to the total score of the move

        Args:
            points (int): the points
        """
        self._score += points

    def get_move(self):
        """Returns the move x,y tuple

        Returns:
            tuple: the move
        """
        return self._move

    def get_score(self):
        """Returns the move's score

        Returns:
            int: total score
        """
        return self._score

    def get_info(self):
        """Descritption of the move as described by info lines

        Returns:
            str: the info string
        """
        return self._info.format(
            x=self._move[0],
            y=self._move[1],
            score=self._score)+"\n"

    def __gt__(self, other):
        """Compares two moves according to their score

        Args:
            other (ExtendedMove): the one compared against

        Returns:
            bool: does self have bigger score than other?
        """
        return self._score > other.get_score()

    def __str__(self):
        return self.get_info()


class Agent:
    def __init__(self, board_ref=None, play_as_side=OPPONENT_SIDE):
        """Creates the agent instance

        Args:
            board_ref (Board, optional): Reference to the game board.
                Is optional. During the game life-cycle it is set when the
                game board is ready. Defaults to None.
            play_as_side (int, optional): Side code of the side on which the
                AI is supposed to play. Defaults to OPPONENT_SIDE.
        """
        self._board = board_ref
        self._side = play_as_side

    def get_next_move(self):
        """Checks if the board is assigned for all overriding functions

        Raises:
            NoBoardReferenceError: If board was not set
        """
        if not self._board:
            raise NoBoardReferenceError()

    def set_board_ref(self, board_ref):
        """Setts the board reference

        Args:
            board_ref (Board): reference to the board of the game
        """
        self._board = board_ref

    def get_board(self):
        """Returns the referenced board

        Returns:
            Board: the board reference
        """
        return self._board


class DumBot(Agent):
    """
    Artificial Intelligence Agent, whose decicion making skills
    are based on pure chance alone.
    """

    def get_next_move(self):
        """Chooses a random available move

        Returns:
            ExtendedMove: the choice
        """
        super().get_next_move()
        move = random.choice(list(self.get_board().get_available_connectors()))
        out = ExtendedMove(move, info="Dumbots guess @({x},{y})")
        return out


class HeuriBot(Agent):
    """
    AI Agent, whoose decicions are based on various properties derived from the
    FAU graph, such as the span of the connections, their benefit to the agent
    as well as their detrement to the player.
    """

    def __init__(self, board_ref=None, play_as_side=OPPONENT_SIDE):
        """Creates the heuribot instance

        Args:
            board_ref (Board, optional): Reference to the game board.
                Is optional. During the game life-cycle it is set when the
                game board is ready. Defaults to None.
            play_as_side (int, optional): Side code of the side on which the
                AI is supposed to play. Defaults to OPPONENT_SIDE.
        """
        super().__init__(board_ref, play_as_side)
        self.AI_LENGTH_DELTA_OFFSET = 10
        self.AI_LENGTH_DELTA_COEF = 20
        self.AI_INF = 10000000000000000000000000000000000000000000000000000000
        self.AI_BORDER_CONNECT_BONUS = 10
        self.AI_SAME_MASTER_PENALTY = -10
        self.AI_CONTAINED_BONUS_OFFSET = 5
        self.AI_RELATIVE_IMPORTANCE = 1.2
        self.AI_LENGTH_COEF = 10

    def get_next_move(self):
        """Chooses the best move, with the maximum score.

        Returns:
            ExtendedMove: heuristically best move
        """
        super().get_next_move()
        best_move = None
        for x, y in self.get_board().get_available_connectors():
            move = self.calculate_total_score_of_pos(x, y)
            if not best_move or move > best_move:
                best_move = move
        return best_move

    def calculate_move(self, x, y, side):
        """Calculates the score in relation to the side of the connection
        placed at (x, y)

        Args:
            x (int): x position of connection
            y (int): y position of connection
            side (int): side code

        Returns:
            ExtendedMove: the calculated move with all its info
        """
        move = ExtendedMove((x, y), info="Heuribot chooses ({x},{y}) because:")

        # Getting all the needed data
        n = self.get_board().get_n()
        direction = get_direction(x, y, side)
        a, b = self.get_board().get_neighbouring_statics(x, y, direction)

        a_master_vertex = self.get_board().get_master_vertex_of_static(a)
        b_master_vertex = self.get_board().get_master_vertex_of_static(b)
        field_master_ids = [
            a_master_vertex.id,
            b_master_vertex.id
        ]

        if side == OPPONENT_SIDE:
            # Care for group height
            a_min, a_max = a_master_vertex.get_y_min_max()
            b_min, b_max = b_master_vertex.get_y_min_max()
        elif side == PLAYER_SIDE:
            # Care for group width
            a_min, a_max = a_master_vertex.get_x_min_max()
            b_min, b_max = b_master_vertex.get_x_min_max()

        max_of_maxes = max(a_max, b_max)
        min_of_mins = min(a_min, b_min)

        # Get group lengths
        a_len = a_max-a_min
        b_len = b_max-b_min
        combined_len = max_of_maxes-min_of_mins

        border_fields = [
            self.get_board().UP
            if side == OPPONENT_SIDE else self.get_board().LEFT,
            self.get_board().DOWN
            if side == OPPONENT_SIDE else self.get_board().RIGHT
        ]

        border_ids = [
            border_fields[0].get_id(),
            border_fields[1].get_id()
        ]

        neither_connected_to_minus_border = (
            not border_ids[0] in field_master_ids)
        neither_connected_to_plus_border = (
            not border_ids[1] in field_master_ids)

        same_master = (a_master_vertex == b_master_vertex)
        if same_master:
            # If the connection is obsolete, reduce score but dont set to -inf,
            # because it can be very good when the other direction is taken
            # into account!
            move.add_score(self.AI_SAME_MASTER_PENALTY)
            move.add_info_line(
                f"\tGroup self connection: {self.AI_SAME_MASTER_PENALTY}")

        if not same_master:
            # Diffetent masters
            if a_master_vertex.id in border_ids and\
                    b_master_vertex.id in border_ids:
                # Connection which guarantees a win, score set to +inf
                move.add_score(self.AI_INF)
                move.add_info_line("\tWinning move :)")
            elif a_master_vertex.id in border_ids or\
                    b_master_vertex in border_ids:
                # Connects a group to one of the borders
                move.add_score(self.AI_BORDER_CONNECT_BONUS)
                move.add_info_line(
                    f"\tBorder connection: +{self.AI_BORDER_CONNECT_BONUS}")

            # Group lengths scores
            if combined_len > max(a_len, b_len):
                # If the total span of the group along the
                # direction is increased add points loosly
                # based on length delta
                score = (combined_len - max(a_len, b_len)) / n * \
                    self.AI_LENGTH_DELTA_COEF + self.AI_LENGTH_DELTA_OFFSET
                move.add_score(score)
                move.add_info_line(f"\tCombined group is longer: +{score}")
                score = combined_len / n * self.AI_LENGTH_COEF
                move.add_score(score)
                move.add_info_line(f"\tCombined group's length: +{score}")
            else:
                # If one group is contained within another in terms of their
                # span, (eg. in case the enemy blocks the optimal routes)
                # choose the ones closest to the bigger group's min or max
                delta = -min(
                    abs(max_of_maxes-min(a_max, b_max)
                        ) if neither_connected_to_plus_border else n*2,
                    abs(min_of_mins-max(a_min, b_min)
                        ) if neither_connected_to_minus_border else n*2,
                )
                score = (delta + n) / n * 5 + self.AI_CONTAINED_BONUS_OFFSET
                move.add_score(score)
                move.add_info_line(
                    f"\tThe subgroup in an optimal place: +{score}")
        move.add_info_line("Giving it the final score of {score}")
        return move

    def calculate_total_score_of_pos(self, x, y):
        """For a given connection at (x, y) adds its scores in relation to
        both PLAYER_SIDE and OPPONENT_SIDE to estimate its importance to
        the Agent's side.

        Args:
            x (int): x position
            y (int): y position

        Returns:
            ExtendedMove: combined move for both sides
        """
        player = self.calculate_move(x, y, PLAYER_SIDE)
        opponent = self.calculate_move(x, y, OPPONENT_SIDE)

        final_score = 0
        final_score += player.get_score() * \
            (self.AI_RELATIVE_IMPORTANCE if not self._side == OPPONENT_SIDE
                else 1.0)
        final_score += opponent.get_score() * \
            (self.AI_RELATIVE_IMPORTANCE if self._side == OPPONENT_SIDE
                else 1.0)

        combined = ExtendedMove(
            (x, y),
            final_score,
            "Combined score @({x},{y}) -> {score}\n" +
            "OPPONENT BENEFITS:\n" +
            opponent.get_info() +
            "PLAYER DAMAGE:\n" +
            player.get_info())

        return combined


class SwitchBot(Agent):
    """
    A combination of DumBot and HeuriBot controlled with a fraction
    """

    def __init__(self, fraction, board_ref=None, play_as_side=OPPONENT_SIDE):
        """Creates SwitchBot instance

        Args:
            fraction (float): what fraction of moves should be performed
                by heuribot?
            board_ref (Board): reference to the game board, optional
        """
        super().__init__(board_ref, play_as_side)
        if not 0.0 <= fraction <= 1.0:
            raise InvalidFractionError(fraction)
        self._fraction = fraction
        self._dumbot = DumBot()
        self._heuribot = HeuriBot()

    def set_board_ref(self, board_ref):
        super().set_board_ref(board_ref)
        self._dumbot.set_board_ref(board_ref)
        self._heuribot.set_board_ref(board_ref)

    def get_next_move(self):
        """Chooses the next move between dumbot and heuribot based on the fraction

        Returns:
            ExtendedMove: the chosen move
        """
        super().get_next_move()
        chance = random.random()
        if chance < self._fraction or self._fraction >= 1.0:
            return self._heuribot.get_next_move()
        else:
            return self._dumbot.get_next_move()

    def get_fraction(self):
        """Returns switchbots fraction

        Returns:
            float: ai fraction
        """
        return self._fraction

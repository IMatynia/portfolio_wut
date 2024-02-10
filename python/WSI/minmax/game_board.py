import numpy as np
from constants import NO_PLAYER, P_MAX, P_MIN


class BoardState:
    def __init__(self, n) -> None:
        self.n = n
        self._contents = np.full([n, n], NO_PLAYER)
        self._avail_moves = set()
        for i in range(n):
            for j in range(n):
                self._avail_moves.add((i, j))
        self._last_move = None

        # Heuristics/Win vars
        self._col_coverage = [{P_MAX: 0, P_MIN: 0} for i in range(n)]
        self._row_coverage = [{P_MAX: 0, P_MIN: 0} for i in range(n)]

        self._cross_ulbr = {P_MAX: 0, P_MIN: 0}  # upper left bottom right
        self._cross_blur = {P_MAX: 0, P_MIN: 0}  # bottom left upper right

    def put_move(self, x: int, y: int, player: int):
        if not self._contents[x][y] == NO_PLAYER:
            raise Exception(f"Move at {x};{y} is taken")

        # heuristic vars
        self._col_coverage[x][player] += player
        self._row_coverage[y][player] += player
        if x == y:
            self._cross_ulbr[player] += player
        if x == self.n - y - 1:
            self._cross_blur[player] += player

        self._contents[x][y] = player
        self._avail_moves.remove((x, y))
        self._last_move = (x, y)

    def revert_move(self, x: int, y: int):
        cur_player = self._contents[x][y]
        if cur_player == NO_PLAYER:
            raise Exception(f"No move to revert at {x};{y}")

        # heuristic vars
        self._col_coverage[x][cur_player] -= cur_player
        self._row_coverage[y][cur_player] -= cur_player
        if x == y:
            self._cross_ulbr[cur_player] -= cur_player
        if x == self.n - y - 1:
            self._cross_blur[cur_player] -= cur_player

        self._contents[x][y] = NO_PLAYER
        self._avail_moves.add((x, y))

    def get_available_moves(self):
        return list(self._avail_moves)

    def moves_remaining(self):
        return len(self._avail_moves)

    def check_win_at_last(self):
        if self._last_move is None:
            return False
        else:
            x, y = self._last_move
            return self.check_win_at_pos(x, y)

    def check_win_at_pos(self, x: int, y: int):
        player = self._contents[x][y]
        if player == NO_PLAYER:
            return False

        win = False

        # ulbr cross
        if x == y:
            win |= abs(self._cross_ulbr[player]) == self.n

        # blur cross
        if x == self.n - y - 1:
            win |= abs(self._cross_blur[player]) == self.n

        # horizontal
        win |= abs(self._row_coverage[y][player]) == self.n

        # vertical
        win |= abs(self._col_coverage[x][player]) == self.n

        return win

    def heuristic_score(self):
        score = 0
        for i in range(self.n):
            col_p_min = self._col_coverage[i][P_MIN]
            col_p_max = self._col_coverage[i][P_MAX]

            row_p_min = self._row_coverage[i][P_MIN]
            row_p_max = self._row_coverage[i][P_MAX]

            # Tylko jeden typ w danej kolumnie/wierszu
            if col_p_min*col_p_max == 0:
                score += (col_p_min + col_p_max)
            if row_p_max*row_p_min == 0:
                score += (row_p_min + row_p_max)

        # ulbr cross
        if self._cross_ulbr[P_MAX] * self._cross_ulbr[P_MAX] == 0:
            score += (self._cross_ulbr[P_MAX] + self._cross_ulbr[P_MAX])

        # blur cross
        if self._cross_blur[P_MAX] * self._cross_blur[P_MAX] == 0:
            score += (self._cross_ulbr[P_MAX] + self._cross_ulbr[P_MAX])

        score /= (self.n*(2*self.n+2))
        return score

    def __str__(self) -> str:
        def player_to_char(player):
            if player == P_MAX:
                return "X"
            elif player == P_MIN:
                return "O"
            else:
                return " "

        trans_con = self._contents.T

        rows = ["│".join(list(map(player_to_char, row)))
                for row in trans_con]
        separator = "┼".join(["─"]*self.n)

        return f"\n{separator}\n".join(rows)

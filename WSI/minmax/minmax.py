from game_board import BoardState
from constants import P_MAX, P_MIN
import random
from math import inf


def minimax_init(board: BoardState, depth: int, d_max: int, player: int, alpha: float, beta: float):
    # Terminal states
    if board.check_win_at_last():
        return player*-1
    
    if depth >= d_max:
        return board.heuristic_score()

    currently_available = board.get_available_moves()
    if len(currently_available) == 0:
        # Remis
        return 0

    best_result = None

    if player == P_MAX:
        best_result = -inf
        for move in currently_available:
            x, y = move
            board.put_move(x, y, player)
            result = minimax_init(board,
                                  depth + 1,
                                  d_max,
                                  player*-1,
                                  alpha,
                                  beta)
            board.revert_move(x, y)

            best_result = max(best_result, result)
            alpha = max(alpha, best_result)
            if beta <= alpha:
                break
    else:
        best_result = inf
        for move in currently_available:
            x, y = move
            board.put_move(x, y, player)
            result = minimax_init(board,
                                  depth + 1,
                                  d_max,
                                  player*-1,
                                  alpha,
                                  beta)
            board.revert_move(x, y)

            best_result = min(best_result, result)
            beta = min(beta, best_result)
            if beta <= alpha:
                break
    return best_result


def minmax_competition(board_state: BoardState, player: int,  p_min_depth: int, p_max_depth: int):
    assert p_min_depth > 0 and p_max_depth > 0
    while board_state.moves_remaining() > 0:
        best_result = None
        best_moves_ties = []
        currently_available = board_state.get_available_moves()

        if player == P_MAX:
            best_result = -inf
            for move in currently_available:
                x, y = move
                board_state.put_move(x, y, player)
                result = minimax_init(board_state,
                                      0,
                                      p_max_depth-1,
                                      player*-1,
                                      -inf,
                                      inf)
                board_state.revert_move(x, y)
                if result > best_result:
                    best_result = result
                    best_moves_ties = [move]
                elif result == best_result:
                    best_moves_ties.append(move)
        elif player == P_MIN:
            best_result = inf
            for move in currently_available:
                x, y = move
                board_state.put_move(x, y, player)
                result = minimax_init(board_state,
                                      0,
                                      p_min_depth-1,
                                      player*-1,
                                      -inf,
                                      +inf)
                board_state.revert_move(x, y)
                if result < best_result:
                    best_result = result
                    best_moves_ties = [move]
                elif result == best_result:
                    best_moves_ties.append(move)

        # choose a random one that tied
        x, y = random.choice(best_moves_ties)
        board_state.put_move(x, y, player)
        if board_state.check_win_at_pos(x, y):
            return player
        player *= -1

    # Noone won and the board is full
    return 0

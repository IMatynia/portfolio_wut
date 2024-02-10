from game_board import BoardState
from constants import P_MAX, P_MIN, NO_PLAYER
from tests.util import *


def test_ULBR_win():
    b = BoardState(3)
    place_move_array(b, [
        (0, 0, P_MAX),
        (1, 1, P_MAX),
        (2, 2, P_MAX),
    ])
    assert b.check_win_at_pos(2, 2)
    assert b.check_win_at_last()
    assert b.check_win_at_pos(1, 1)
    assert not b.check_win_at_pos(0, 2)

    b.revert_move(1, 1)
    assert not b.check_win_at_pos(2, 2)
    assert not b.check_win_at_last()


def test_BLUR_win():
    b = BoardState(3)
    place_move_array(b, [
        (2, 0, P_MAX),
        (1, 1, P_MAX),
        (0, 2, P_MAX),
    ])
    assert b.check_win_at_pos(2, 0)
    assert b.check_win_at_last()
    assert b.check_win_at_pos(1, 1)


def test_BLUR_even():
    b = BoardState(4)
    place_move_array(b, [
        (3, 0, P_MIN),
        (2, 1, P_MIN),
        (1, 2, P_MIN),
        (0, 3, P_MIN),
    ])
    assert b.check_win_at_pos(1, 2)


def test_col_row_wins():
    b = BoardState(3)
    place_move_array(b, [
        (1, 0, P_MIN),
        (1, 1, P_MIN),
        (1, 2, P_MIN),
        (0, 1, P_MIN),
        (2, 1, P_MIN),
    ])
    assert b.check_win_at_last()
    assert b.check_win_at_pos(1, 0)
    assert not b.check_win_at_pos(0, 0)
    assert b.check_win_at_pos(0, 1)


def test_moves():
    b = BoardState(4)
    assert (1, 1) in b._avail_moves
    b.put_move(1, 1, P_MAX)
    assert not (1, 1) in b._avail_moves
    assert b._contents[1][1] == P_MAX
    b.revert_move(1, 1)
    assert (1, 1) in b._avail_moves
    assert b._contents[1][1] == NO_PLAYER


def test_odd():
    b = BoardState(3)
    b.put_move(0, 1, -1)
    b.put_move(1, 1, -1)
    b.put_move(1, 2, -1)
    b.put_move(0, 2, 1)
    b.put_move(2, 0, 1)
    b.put_move(1, 0, 1)
    b.put_move(0, 0, 1)
    assert b.check_win_at_last()

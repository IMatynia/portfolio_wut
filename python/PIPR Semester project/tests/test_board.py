import pytest

from game_code.board import (
    Board, Connector, StaticVertex, get_direction,
    OccupiedConnectorError, InvalidCoordinateError, InvalidDirectionError
)
from game_code.constants import (
    HORIZONTAL,
    NO_DIRECTION,
    NO_SIDE,
    OPPONENT_SIDE,
    PLAYER_SIDE,
    VERTICAL)


def test_connector_init():
    conn = Connector(5, 1, 1)
    assert conn.get_side() == NO_SIDE
    assert conn.get_direction() == NO_DIRECTION
    assert conn.get_id() == 6


def test_connector_direction_switching():
    conn = Connector(5, 1, 1)

    conn.set_direction(VERTICAL)
    assert conn.get_side() == PLAYER_SIDE
    assert conn.get_direction() == VERTICAL

    conn.set_direction(HORIZONTAL)
    assert conn.get_side() == OPPONENT_SIDE
    assert conn.get_direction() == HORIZONTAL


def test_connector_what_to_display():
    conn = Connector(5, 0, 0)
    assert conn.what_to_display() == ""
    conn.set_direction(HORIZONTAL)
    assert conn.what_to_display() == "⇆"
    conn.set_direction(VERTICAL)
    assert conn.what_to_display() == "⇅"

    stat_vert_player = StaticVertex(5, 1, 0, PLAYER_SIDE)
    assert stat_vert_player.what_to_display() == "YOU"
    stat_vert_opp = StaticVertex(5, 0, 1, OPPONENT_SIDE)
    assert stat_vert_opp.what_to_display() == "OPP"


def test_static_v_init():
    static = StaticVertex(5, 2, 1, OPPONENT_SIDE)
    assert static.get_side() == OPPONENT_SIDE
    assert static.get_id() == 7
    vert = static.get_vertex()
    assert vert.id == 7
    assert vert.master == 7
    assert vert.x == 2
    assert vert.y == 1
    assert vert.y_min == 1
    assert vert.y_max == 1
    assert vert.x_max == 2
    assert vert.x_min == 2


def test_board_init_no_connections():
    n = 5
    brd = Board(n)
    assert brd.get_n() == n
    assert len(brd.get_available_connectors()) == (n*n)//2+1
    assert brd.check_win() == NO_SIDE

    assert brd.get_board_field(0, 0).get_side() == NO_SIDE
    assert brd.get_board_field(0, 0).get_direction() == NO_DIRECTION
    assert type(brd.get_board_field(0, 0)) == Connector

    assert brd.get_board_field(1, 0).get_side() == PLAYER_SIDE
    assert type(brd.get_board_field(1, 0)) == StaticVertex

    assert brd.get_board_field(0, 1).get_side() == OPPONENT_SIDE
    assert type(brd.get_board_field(0, 1)) == StaticVertex


def test_board_helper_funcs():
    brd = Board(5)
    a, b = brd.get_neighbouring_statics(0, 0, HORIZONTAL)
    assert brd.get_board_field(-1, 0) == a
    assert brd.get_board_field(1, 0) == b

    assert brd.get_available_connectors() == set(
        [(4, 4), (2, 4), (4, 0), (0, 4), (0, 0), (3, 1), (1, 1),
         (2, 0), (4, 2), (0, 2), (3, 3), (2, 2), (1, 3)]
    )

    assert get_direction(0, 0, OPPONENT_SIDE) == VERTICAL
    assert get_direction(1, 1, OPPONENT_SIDE) == HORIZONTAL
    assert get_direction(0, 0, PLAYER_SIDE) == HORIZONTAL
    assert get_direction(1, 1, PLAYER_SIDE) == VERTICAL


def test_board_border_verticies():
    n = 5
    brd = Board(n)

    assert brd.get_board_field(-1, 0) == brd.LEFT
    assert brd.get_board_field(-1, 0).get_side() == PLAYER_SIDE
    assert brd.get_board_field(n, 0) == brd.RIGHT
    assert brd.get_board_field(n, 0).get_side() == PLAYER_SIDE
    assert brd.get_board_field(0, -1) == brd.UP
    assert brd.get_board_field(0, -1).get_side() == OPPONENT_SIDE
    assert brd.get_board_field(0, n) == brd.DOWN
    assert brd.get_board_field(0, n).get_side() == OPPONENT_SIDE


def test_board_add_connections():
    n = 5
    brd = Board(n)
    brd.place_connection(0, 0, VERTICAL)
    brd.place_connection(2, 0, HORIZONTAL)
    brd.place_connection(2, 2, VERTICAL)
    brd.place_connection(1, 1, HORIZONTAL)
    id_up = brd.UP.get_id()
    assert brd.get_master_id_of_static(brd.get_board_field(1, 0)) == 1
    assert brd.get_master_id_of_static(brd.get_board_field(3, 0)) == 1

    assert brd.get_master_id_of_static(brd.get_board_field(0, 1)) == id_up
    assert brd.get_master_id_of_static(brd.get_board_field(2, 1)) == id_up
    assert brd.get_master_id_of_static(brd.get_board_field(2, 3)) == id_up


def test_board_add_connection_occupied_error():
    brd = Board(5)
    brd.place_connection(0, 0, VERTICAL)

    with pytest.raises(OccupiedConnectorError):
        brd.place_connection(0, 0, VERTICAL)

    with pytest.raises(OccupiedConnectorError):
        brd.place_connection(0, 0, HORIZONTAL)


def test_board_add_connection_invalid_coordinate_error():
    brd = Board(5)

    with pytest.raises(InvalidCoordinateError):
        brd.place_connection(1, 0, VERTICAL)

    with pytest.raises(InvalidCoordinateError):
        brd.place_connection(131223, -213131, VERTICAL)


def test_board_add_connection_invalid_direction_error():
    brd = Board(5)

    with pytest.raises(InvalidDirectionError):
        brd.place_connection(1, 1, 7126391281)

    with pytest.raises(InvalidDirectionError):
        brd.place_connection(0, 0, -123123123213)


def test_board_save_load_state():
    n = 5
    brd = Board(n)
    brd.place_connection(3, 3, HORIZONTAL)
    brd.place_connection(4, 2, VERTICAL)
    brd.place_connection(4, 4, HORIZONTAL)
    state = brd.dump_state()
    assert state == [
        {'x': 4, 'y': 2, 'direction': -1},
        {'x': 3, 'y': 3, 'direction': 1},
        {'x': 4, 'y': 4, 'direction': 1}
    ]

    brd2 = Board(n)
    brd2.load_state(state)
    assert brd2.dump_state() == state
    assert brd2.get_available_connectors() == brd.get_available_connectors()
    assert brd2.check_win() == brd.check_win()


def test_win_simple():
    n = 5
    brd = Board(n)
    assert brd.check_win() == NO_SIDE
    brd.place_connection(0, 0, HORIZONTAL)
    assert brd.check_win() == NO_SIDE
    brd.place_connection(2, 0, HORIZONTAL)
    assert brd.check_win() == NO_SIDE
    brd.place_connection(4, 0, HORIZONTAL)
    assert brd.check_win() == PLAYER_SIDE

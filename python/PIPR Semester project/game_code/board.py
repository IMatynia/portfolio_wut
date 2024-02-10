import game_code.FAU as FAU
from game_code.constants import (
    HORIZONTAL, NO_DIRECTION, NO_SIDE, OPPONENT_SIDE,
    PLAYER_SIDE, VALID_DIRECTIONS, VALID_SIDES, VERTICAL)

# Exceptions: #---------------------------------------------------------------#


class InvalidBoardSizeError(Exception):
    def __init__(self, n):
        self.n = n
        super().__init__(f"{self.n} is an invalid board size\n" +
                         "It must be odd and greater or equal to 3")


class InvalidSideError(Exception):
    def __init__(self, side):
        super().__init__(f"{side} is not a valid side code!")
        self.side = side


class InvalidDirectionError(Exception):
    def __init__(self, direction):
        super().__init__(f"{direction} is not a valid direction code!")
        self.direction = direction


class InvalidCoordinateError(Exception):
    def __init__(self, x, y):
        super().__init__(f"({x};{y}) are not valid coordinates here")
        self.x = x
        self.y = y


class OccupiedConnectorError(Exception):
    def __init__(self, x, y):
        super().__init__(f"Connector at ({x};{y}) is occupied!")
        self.x = x
        self.y = y

# Classes: #---------------------------------------------------------------#


class Board:
    def __init__(self, n):
        """Creates a board of size n x n with no connections

        Args:
            n (int): Board size, must be odd and greater than 0

        Raises:
            InvalidBoardSizeError: If invalid n is provided
        """
        if n % 2 == 0 or n < 1:
            raise InvalidBoardSizeError(n)

        self._n = n
        self._fields = []
        self._fau = FAU.FAUGraph()
        self._available_connectors = set([])

        self._bind_borders_to_fau()

        for y in range(n):
            row = []
            for x in range(n):
                if (x+y) % 2 == 0:
                    # Adding empty connectors
                    row.append(Connector(n, x, y))
                    self._available_connectors.add((x, y))
                elif y % 2 == 0:
                    # Adding static vertexes for the player
                    static = StaticVertex(n, x, y, side=PLAYER_SIDE)
                    self._fau.add_vertex(static.get_vertex())

                    row.append(static)
                else:
                    # Adding static vertexes for the opponent
                    static = StaticVertex(n, x, y, side=OPPONENT_SIDE)
                    self._fau.add_vertex(static.get_vertex())

                    row.append(static)
            self._fields.append(row)

    def check_win(self):
        """Checks if any side won the game, returns code NO_SIDE if there
        is no winner yet

        Returns:
            int: side code of the side that won
        """
        if self.get_master_id_of_static(self.UP) ==\
                self.get_master_id_of_static(self.DOWN):
            return OPPONENT_SIDE
        elif self.get_master_id_of_static(self.LEFT) ==\
                self.get_master_id_of_static(self.RIGHT):
            return PLAYER_SIDE
        else:
            return NO_SIDE

    def place_connection(self, x, y, direction):
        """Connects static vertexes adjacent to the connector at (x,y) in a
        given direction. Valid directions are specified in constants.py

        Args:
            x (int): x coordinate of the connector
            y (int): y coordinate of the connector
            direction (int): direction code from constants.py

        Raises:
            InvalidCoordinateError: If coordinates to do not
            point at a connector
            OccupiedConnectionError: If a connection is already occupied
        """
        conn = self.get_board_field(x, y)

        if not type(conn) == Connector:
            raise InvalidCoordinateError(x, y)

        if not conn.get_direction() == NO_DIRECTION:
            raise OccupiedConnectorError(x, y)
        conn.set_direction(direction)

        self._available_connectors.remove((x, y))

        sv_a, sv_b = self.get_neighbouring_statics(x, y, direction)

        id_a = sv_a.get_vertex().id
        id_b = sv_b.get_vertex().id
        self._fau.add_edge(id_a, id_b)

    def get_neighbouring_statics(self, x, y, direction):
        """Returns 2 neighbouring StaticVertex'es along a given direction

        Args:
            x (int): x position
            y (int): y position
            direction (int): direction code

        Raises:
            InvalidDirectionError: If invalid direction given

        Returns:
            tuple(StaticVertex, StaticVertex): the neighbouring statics
        """
        if direction == VERTICAL:
            sv_a = self.get_board_field(x, y-1)
            sv_b = self.get_board_field(x, y+1)
        elif direction == HORIZONTAL:
            sv_a = self.get_board_field(x-1, y)
            sv_b = self.get_board_field(x+1, y)
        else:
            raise InvalidDirectionError(direction)
        return sv_a, sv_b

    def get_board_field(self, x, y):
        """Returns a reference to a board field of given coordinates

        Args:
            x (int): x location <-1;n>
            y (int): y location <-1;n>

        Raises:
            InvalidCoordinateError: If coordinates do not point to
            a valid field

        Returns:
            BoardField: The field at given coordinates
        """
        if not (self._n >= x >= -1 and self._n >= y >= -1):
            raise InvalidCoordinateError(x, y)

        if x == -1:
            return self.LEFT
        if x == self._n:
            return self.RIGHT

        if y == -1:
            return self.UP
        if y == self._n:
            return self.DOWN

        return self._fields[y][x]

    def get_master_id_of_static(self, statVert):
        """Returns field's master id

        Args:
            statVert (StaticVertex): the field

        Returns:
            int: Master's ID
        """
        if type(statVert) != StaticVertex:
            raise TypeError("statVert must me a static vertex")
        master_id = self._fau.find_master_id(statVert.get_vertex().id)
        return master_id

    def get_master_vertex_of_static(self, statVert):
        """Returns field's master vertex

        Args:
            statVert (StaticVertex): the field

        Returns:
            Vertex: Master vertex
        """
        if type(statVert) != StaticVertex:
            raise TypeError("statVert must me a static vertex")
        master_vert = self._fau.find_master_vertex(statVert.get_vertex().id)
        return master_vert

    def get_n(self):
        """Returns the board size

        Returns:
            int: n
        """
        return self._n

    def get_available_connectors(self):
        """
        Returns a list of coordinates of connectors
        whoose direction is set to NO_DIRECTION
        """
        return self._available_connectors

    def _bind_borders_to_fau(self):
        """
        Creates and adds border verticies to FAU graph so that connections
        to the opposite ends of the board can be registered
        All border verticies have negative IDs to give them priority in
        becoming masters
        """
        self.UP = StaticVertex(self._n, 0, -1, OPPONENT_SIDE, id=-1)
        self.DOWN = StaticVertex(self._n, 0, self._n, OPPONENT_SIDE, id=-2)
        self.LEFT = StaticVertex(self._n, -1, 0, PLAYER_SIDE, id=-3)
        self.RIGHT = StaticVertex(self._n, self._n, 0, PLAYER_SIDE, id=-4)

        self._fau.add_vertex(self.UP.get_vertex())
        self._fau.add_vertex(self.DOWN.get_vertex())
        self._fau.add_vertex(self.LEFT.get_vertex())
        self._fau.add_vertex(self.RIGHT.get_vertex())

    def dump_state(self):
        """Dumps all relevant information needed to reconstruct the board,
        which turn out to be connections.
        """
        connections = []
        for row in self._fields:
            for field in row:
                if type(field) == Connector and\
                        field.get_direction() != NO_DIRECTION:
                    pos_x, pos_y = field.get_position()
                    direction = field.get_direction()
                    connections.append({
                        "x": pos_x,
                        "y": pos_y,
                        "direction": direction
                    })
        return connections

    def load_state(self, connections):
        """Places all given connections

        Args:
            connections (list(dict)): connections to be placed
        """
        for connection in connections:
            x = connection["x"]
            y = connection["y"]
            direction = connection["direction"]
            self.place_connection(x, y, direction)


class BoardField:
    """Defines a field on the game board
    """

    def __init__(self, n, pos_x, pos_y, id=None):
        """Creates field instance

        Args:
            n (int): size of the board
            pos_x (int): x coordinate
            pos_y (int): y coordinate
            id (int, optional): custom id. Defaults to an automatically
                generated one.
        """
        self._n = n
        self._pos_x = pos_x
        self._pos_y = pos_y
        self._id = self._n*self._pos_y+self._pos_x if id is None else id

    def _get_n(self):
        """Returns the size of the board the field is on

        Returns:
            int: n
        """
        return self._n

    def get_position(self):
        """Returns field's position

        Returns:
            tuple: (x, y) position
        """
        return (self._pos_x, self._pos_y)

    def get_id(self):
        """Returns board field's id

        Returns:
            int: id
        """
        return self._id

    def what_to_display():
        """Returns what string the field should be displayed as.

        Returns:
            str: The display string.
        """
        pass

    def info(self):
        """Returns a description of the board field

        Returns:
            str: description
        """
        return f"@(x={self._pos_x},y={self._pos_y})\n"

    def __str__(self):
        return self.info()


class Connector(BoardField):
    def __init__(self, n, pos_x, pos_y, id=None):
        """Creates connector instance
        directions defaults to NO_DIRECTION

        Args:
            n (int): size of the board
            pos_x (int): x coordinate
            pos_y (int): y coordinate
            id (int, optional): custom id. Defaults to an automatically
                generated one.
        """
        super().__init__(n, pos_x, pos_y, id=id)
        self._direction = NO_DIRECTION

    def get_direction(self):
        """Returns connector's direction code

        Returns:
            int: direction code
        """
        return self._direction

    def get_side(self):
        """As weird as it may seem, this works and is one of the
        easiest ways to do this. Returns the side code of the side
        which placed the connection. Returns NO_SIDE code if not occupied yet

        Returns:
            int: side code
        """
        pos_x, pos_y = self.get_position()
        if pos_y % 2 == 0:
            return self.get_direction()
        else:
            return -self.get_direction()

    def reset(self):
        """Resets connectors direction to the default of NO_DIRECTION
        """
        self._direction = NO_DIRECTION

    def set_direction(self, direction):
        """Setts the direction of the connector

        Args:
            direction (int): direction code

        Raises:
            InvalidDirectionError: if invalid direction given
        """
        direction = int(direction)
        if direction not in VALID_DIRECTIONS:
            raise InvalidDirectionError(direction)
        self._direction = direction

    def info(self):
        return super().info()+f"Connector in direction {self._direction}"

    def what_to_display(self):
        return "⇆" if self._direction == HORIZONTAL else\
            "⇅" if self._direction == VERTICAL else ""


class StaticVertex(BoardField):
    """
    Unchanging field on the board defining a vertex of a given side
    Automatically creates its analogue of _vert which is added to the FAU graph
    """

    def __init__(self, n, pos_x, pos_y, side, id=None):
        """Creates static vertex instance.

        Args:
            n (int): size of the board
            pos_x (int): x coordinate
            pos_y (int): y coordinate
            side (int): side code of the vertex
            id (int, optional): custom id. Defaults to an automatically
                generated one.
        """
        super().__init__(n, pos_x, pos_y, id=id)
        self._vert = FAU.Vertex(self.get_id(), pos_x, pos_y)

        if side not in VALID_SIDES:
            raise InvalidSideError(side)
        self._side = side

    def get_vertex(self):
        """Returns static's FAU vertex

        Returns:
            Vertex: the vertex
        """
        return self._vert

    def get_side(self):
        """Returns static's side code

        Returns:
            int: side code
        """
        return self._side

    def info(self):
        """Returns a description of the static vertex

        Returns:
            str: description
        """
        return super().info() + f"Static Vertex of side {self._side}, " +\
            f"Connected to FAU vertex: {self._vert}"

    def what_to_display(self):
        return "OPP" if self._side == OPPONENT_SIDE else "YOU"


def get_direction(x, y, side):
    """Returns which direction should be a connection of the side
    if put it at (x,y) coordinates

    Args:
        x (int): x position
        y (int): y position
        side (int): side code

    Returns:
        int: direction code
    """
    if side not in VALID_SIDES:
        raise InvalidSideError(side)

    if (y+(side == OPPONENT_SIDE)) % 2 == 0:
        return HORIZONTAL
    else:
        return VERTICAL

class VertexDoesNotExistError(Exception):
    def __init__(self, id):
        super().__init__(f"Vertex of id {id} is not in the graph")
        self.id = id


class Vertex:
    """
    Vertex used in the FAU graph. Stores information about its ID
    and its master's ID
    """

    def __init__(self, id, x_pos, y_pos):
        """Creates a vertex instance

        Args:
            id (int): integer value representing the vertex in the graph,
                special vertecies have negative ids
            x_pos (int): x position on the game board
            y_pos (int): y position on the game board
        """
        self.id = int(id)
        self.x = int(x_pos)
        self.y = int(y_pos)

        # AI will take into account the height and width of the spójna
        self.y_max = self.y
        self.y_min = self.y

        self.x_max = self.x
        self.x_min = self.x

        self.master = int(id)

    def info(self):
        """Returns description of the vertex'es info and its cached data

        Returns:
            str: description
        """
        return f"Vertex id={self.id} | Chache: master={self.master}, " +\
            f"y@<{self.y_min};{self.y_max}>, x@<{self.x_min};{self.x_max}>"

    def get_y_min_max(self):
        """Returns vertex's chached y min/max tuple

        Returns:
            tuple: y min and y max
        """
        return self.y_min, self.y_max

    def get_x_min_max(self):
        """Returns vertex's chached x min/max tuple

        Returns:
            tuple: x min and x max
        """
        return self.x_min, self.x_max

    def __str__(self):
        return self.info()


class FAUGraph:
    """
    Find and union graph class.
    Groups connected verticies onto one master verticie.
    Lowwer ID has a priority in becoming a master.
    """

    def __init__(self):
        """
        Creates an empty graph
        """
        self._graph = {}

    def add_vertex(self, v):
        """Adds a vertex to the graph

        Args:
            v (Vertex): Vertex reference

        Raises:
            ValueError: If a vertex of this id is already present in the graph
        """
        if v.id in self._graph:
            raise ValueError("Vertex of this ID is already in the graph")
        self._graph[v.id] = v

    def add_edge(self, a, b):
        """Joins vertex a's and b's groups and calculate's their new
        y ranges

        Args:
            a (int): a's ID
            b (int): b's ID

        Raises:
            VertexDoesNotExistError: If a vertex is not in the graph
        """
        if a not in self._graph:
            raise VertexDoesNotExistError(a)
        if b not in self._graph:
            raise VertexDoesNotExistError(b)

        master_a = self.find_master_id(a)
        master_b = self.find_master_id(b)

        master_new = min(master_a, master_b)
        y_min_new = min(self._graph[master_a].y_min,
                        self._graph[master_b].y_min)
        y_max_new = max(self._graph[master_a].y_max,
                        self._graph[master_b].y_max)

        x_min_new = min(self._graph[master_a].x_min,
                        self._graph[master_b].x_min)
        x_max_new = max(self._graph[master_a].x_max,
                        self._graph[master_b].x_max)

        self._graph[master_a].master = master_new
        self._graph[master_b].master = master_new

        self._graph[master_new].y_min = y_min_new
        self._graph[master_new].y_max = y_max_new

        self._graph[master_new].x_min = x_min_new
        self._graph[master_new].x_max = x_max_new

    def find_master_vertex(self, id):
        """Returns the master vertex

        Args:
            id (int): id of a child

        Returns:
            Vertex: the master vertex
        """
        return self._graph[self.find_master_id(id)]

    def find_master_id(self, id):
        """Finds the id of the master of the group of
        which vertex of given id is a part of and updates all
        visited verts

        Args:
            id (int): Vertex ID

        Raises:
            VertexDoesNotExistError: If the vertex is not a part of the graph

        Returns:
            int: ID of the found master
        """
        if id not in self._graph:
            raise VertexDoesNotExistError(id)

        if self._graph[id].master == id:
            return id
        else:
            master = self.find_master_id(self._graph[id].master)
            self._graph[id].master = master
            return master

    def get_vertex(self, id):
        """Returns a vertex of the given id

        Args:
            id (int): vertex id

        Raises:
            VertexDoesNotExistError: If invalid id

        Returns:
            Vertex: the vertex
        """
        if id not in self._graph:
            raise VertexDoesNotExistError(id)

        return self._graph[id]

    def full_refresh(self):
        """Udates all vericies and their masters
        """
        for id in self._graph:
            self._graph[id].master = self.find_master_id(id)

    def info(self):
        """Returns a description of the graph

        Returns:
            str: description
        """
        self.full_refresh()
        out = f"FAU Graph of {len(self._graph)} verticies:\n"
        for id in self._graph:
            out += f"\t{self._graph[id]}\n"
        return out

    def __str__(self):
        return self.info()

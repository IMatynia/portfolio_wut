from game_code.FAU import Vertex, FAUGraph
from game_code.FAU import VertexDoesNotExistError
import pytest


def test_vertex_typical():
    v1 = Vertex(0, 1, 2)
    assert v1.id == 0
    assert v1.master == 0
    v1.id = 2
    v1.master = 3
    assert v1.id == 2
    assert v1.master == 3

    assert v1.y_max == 2
    assert v1.y_min == 2

    assert v1.x_max == 1
    assert v1.x_min == 1


def test_graph_init():
    graph = FAUGraph()

    assert graph._graph == {}
    assert graph.info() == 'FAU Graph of 0 verticies:\n'


def test_graph_add_vertex():
    g = FAUGraph()
    v = Vertex(5, 4, 10)
    g.add_vertex(v)
    assert g.get_vertex(5) == v


def test_graph_add_edges():
    g = FAUGraph()
    v1 = Vertex(5, 4, 10)
    v2 = Vertex(7, 5, 18)
    g.add_vertex(v1)
    g.add_vertex(v2)
    g.add_edge(v1.id, v2.id)
    assert g.find_master_id(v1.id) == g.find_master_id(v2.id)


def test_find_master():
    graph = FAUGraph()
    v1 = Vertex(3, 14, 121)
    v2 = Vertex(2, 124, 11)
    v3 = Vertex(10, 154, 21)

    graph.add_vertex(v1)
    graph.add_vertex(v2)
    graph.add_vertex(v3)

    graph.add_edge(v1.id, v3.id)
    assert graph.find_master_id(v1.id) == graph.find_master_id(v3.id)
    assert graph.find_master_vertex(v1.id) == graph.find_master_vertex(v3.id)
    assert graph.find_master_id(v3.id) == v1.id
    assert graph.find_master_vertex(v3.id) == v1

    graph.add_edge(v1.id, v2.id)
    assert graph.find_master_id(v3.id) == graph.find_master_id(v2.id)
    assert graph.find_master_vertex(v3.id) == graph.find_master_vertex(v2.id)
    assert graph.find_master_id(v3.id) == v2.id
    assert graph.find_master_vertex(v3.id) == v2


def test_y_x_min_max():
    g = FAUGraph()
    g.add_vertex(Vertex(1, 0, 3))
    g.add_vertex(Vertex(2, 2, 6))
    g.add_vertex(Vertex(3, 4, 2))
    g.add_vertex(Vertex(4, 6, 5))
    g.add_vertex(Vertex(5, 49, 10))
    g.add_vertex(Vertex(6, 50, 12))

    g.add_edge(3, 4)
    g.add_edge(1, 2)

    # 3-4   1-2
    # Y
    assert g.find_master_vertex(1).get_y_min_max(
    ) == g.find_master_vertex(2).get_y_min_max()
    assert g.find_master_vertex(1).get_y_min_max() == (3, 6)

    assert g.find_master_vertex(3).get_y_min_max(
    ) == g.find_master_vertex(4).get_y_min_max()
    assert g.find_master_vertex(3).get_y_min_max() == (2, 5)
    # X
    assert g.find_master_vertex(1).get_x_min_max(
    ) == g.find_master_vertex(2).get_x_min_max()
    assert g.find_master_vertex(1).get_x_min_max() == (0, 2)

    assert g.find_master_vertex(3).get_x_min_max(
    ) == g.find_master_vertex(4).get_x_min_max()
    assert g.find_master_vertex(3).get_x_min_max() == (4, 6)

    g.add_edge(3, 1)
    # 4-3-1-2
    assert g.find_master_vertex(4).get_y_min_max() == (2, 6)
    assert g.find_master_vertex(4).get_x_min_max() == (0, 6)
    g.add_edge(6, 2)
    # 4-3-1-2-6
    assert g.find_master_vertex(3).get_y_min_max() == (2, 12)
    assert g.find_master_vertex(3).get_x_min_max() == (0, 50)
    g.add_edge(2, 5)
    # 4-3-1-2-5-6
    assert g.find_master_vertex(5).get_y_min_max() == (2, 12)
    assert g.find_master_vertex(5).get_x_min_max() == (0, 50)


def test_example_graph():
    graph = FAUGraph()

    for i in range(8):
        graph.add_vertex(Vertex(i, (i-4)**2, i))

    graph.add_edge(1, 2)
    graph.add_edge(4, 5)
    graph.add_edge(3, 4)
    graph.add_edge(1, 5)
    graph.add_edge(6, 7)

    # 1-2-5-4-3    6-7
    assert graph.find_master_id(1) == 1
    assert graph.find_master_id(2) == 1
    assert graph.find_master_id(3) == 1
    assert graph.find_master_id(4) == 1
    assert graph.find_master_id(5) == 1
    assert graph.find_master_id(6) == 6
    assert graph.find_master_id(7) == 6

    assert graph.get_vertex(1).get_y_min_max() == (1, 5)
    assert graph.get_vertex(6).get_y_min_max() == (6, 7)

    assert graph.get_vertex(1).get_x_min_max() == (0, 9)
    assert graph.get_vertex(6).get_x_min_max() == (4, 9)


def test_detached_verticies():
    graph = FAUGraph()
    v1 = Vertex(1, 0, 0)
    v2 = Vertex(2, 0, 1)
    graph.add_vertex(v1)
    graph.add_vertex(v2)
    assert graph.find_master_id(v1.id) == v1.id
    assert graph.find_master_id(v2.id) == v2.id


def test_invalid_verticies():
    graph = FAUGraph()
    v1 = Vertex(1, 0, 0)
    graph.add_vertex(v1)
    with pytest.raises(VertexDoesNotExistError):
        graph.find_master_id(21381281369216312)

    with pytest.raises(VertexDoesNotExistError):
        graph.add_edge(v1.id, 21313213123)

    with pytest.raises(ValueError):
        graph.add_vertex(Vertex(1, 69, 69))

    assert graph.find_master_id(v1.id) == v1.id

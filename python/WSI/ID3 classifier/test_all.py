from id3classifier import DecisionNode, ClassificationNode, get_recursive_classification


def test_classification_tree():
    term_y = ClassificationNode(True)
    term_n = ClassificationNode(False)

    dec1 = DecisionNode("color")

    dec1.add_child("red", term_y)
    dec1.add_child("green", term_n)
    dec1.add_child("blue", term_n)

    dec2 = DecisionNode("color")

    dec2.add_child("red", term_n)
    dec2.add_child("green", term_n)
    dec2.add_child("blue", term_y)

    dec3 = DecisionNode("size")

    dec3.add_child("1-2", dec1)
    dec3.add_child("3-5", dec2)
    dec3.add_child("5-10", dec2)
    dec3.add_child("10-100", dec1)

    assert get_recursive_classification({"color": "blue", "size": "1-2"}, dec3) == term_n
    assert get_recursive_classification({"color": "blue", "size": "5-10"}, dec3) == term_y


test_classification_tree()
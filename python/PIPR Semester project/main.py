import game_code.gui as gui
import sys
from PySide2.QtWidgets import QApplication
import logging as log

log.basicConfig(
    format="[%(asctime)s->%(levelname)s->%(module)s" +
    "->%(funcName)s]: %(message)s\n",
    datefmt="%H:%M:%S",
    level=log.INFO
)


def gui_main(args):
    app = QApplication(args)
    window = gui.MainWindow(app)
    window.show()
    return app.exec_()


if __name__ == "__main__":
    gui_main(sys.argv)

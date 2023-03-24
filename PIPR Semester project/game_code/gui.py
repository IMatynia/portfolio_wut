from PySide2.QtWidgets import (
    QMainWindow,
    QLabel, QMessageBox, QSizePolicy,
    QStyle, QFileDialog
)
from PySide2.QtCore import QSize, Qt, QTimer
from game_code.UI.Ui_MainWindow import Ui_MainWindow
from game_code.constants import (
    MAIN_MENU_PAGE_INDEX,
    GAME_PAGE_INDEX,
    PLAYER_SIDE,
    OPPONENT_SIDE,
    WIN_PAGE_INDEX,
    LOSS_PAGE_INDEX,
    STATIC_V_QSS,
    CONNECTOR_QSS
)
from game_code.game import GameConfig, GaleShannonGame
from game_code.board import (
    InvalidBoardSizeError,
    InvalidCoordinateError,
    InvalidDirectionError,
    Connector,
    StaticVertex,
    OccupiedConnectorError
)
from game_code.game_data_io import (
    read_game_config_from_json_stream,
    write_game_config_to_stream,
)
from game_code.ai import SwitchBot
from math import tanh, pi
import logging as log
from json import JSONDecodeError


class MainWindow(QMainWindow):
    def __init__(self, app, parent=None):
        """Creates an instance of the game window.
        Everything needed to play the game in UI mode is contained
        in this class.

        Args:
            app (QApp): reference to the app
            parent (QWidget, optional): parent widget. Defaults to None.
        """
        # UI setup
        super().__init__(parent)
        self._app = app
        self._ui = Ui_MainWindow()
        self._ui.setupUi(self)
        self._stylize_ui()

        # Defining variables
        self.game_config = None
        self.board_field_displays = None
        self.game = None
        self._popup_widget = None
        self._clock = None
        self._game_time = None

        # Connect callbacks
        self._ui.opponent_slider.valueChanged.connect(
            self._update_opponent_display)
        self._ui.play_game_button.clicked.connect(self._start_game)

        self._ui.actionNew_game.triggered.connect(self._main_menu)
        self._ui.actionLoad_game.triggered.connect(self._load_game_state)
        self._ui.actionSave_game_state.triggered.connect(self._save_game_state)

        log.info(app.arguments())

        # Open main menu page
        self._main_menu()

    # Tools: #----------------------------------------------------------------#

    def _switch_to_page(self, index):
        """Switches the stack display to the given page
        and sets the actions accordingly

        Args:
            index (int): page number

        Raises:
            ValueError: if invalid page number given
        """
        if index == MAIN_MENU_PAGE_INDEX:
            self._ui.actionNew_game.setDisabled(True)
            self._ui.actionLoad_game.setDisabled(False)
            self._ui.actionSave_game_state.setDisabled(True)
        elif index == GAME_PAGE_INDEX:
            self._ui.actionNew_game.setDisabled(False)
            self._ui.actionLoad_game.setDisabled(False)
            self._ui.actionSave_game_state.setDisabled(False)
        elif index == WIN_PAGE_INDEX or index == LOSS_PAGE_INDEX:
            self._ui.actionNew_game.setDisabled(False)
            self._ui.actionLoad_game.setDisabled(False)
            self._ui.actionSave_game_state.setDisabled(False)
        else:
            raise ValueError("Invalid page")
        self._ui.stack.setCurrentIndex(index)

    def _show_popup(self, title, msg):
        """Shows a popup with a message
        """
        if not self._popup_widget:
            self._popup_widget = QMessageBox(self)
        self._popup_widget.setWindowTitle(title)
        self._popup_widget.setText(msg)
        self._popup_widget.show()

    def _get_game_conf_from_ui(self):
        """Creates a game config out of input fields in the main menu

        Returns:
            GameConfig: the config
        """
        value = self._ui.opponent_slider.value()
        ai_fraction = self._opponent_fraction_curve(value)
        return GameConfig(
            self._ui.spinbox_n.value(),
            SwitchBot(fraction=ai_fraction) if value <= 10 else None,
            PLAYER_SIDE
            if self._ui.player_starts.isChecked() else
            OPPONENT_SIDE
        )

    def _center_and_set_res(self, width, height):
        """Positions the game window in the centre of the screen and sets its scale

        Args:
            width (int): the new width
            height (int): the new height
        """
        screen = self._app.desktop().availableGeometry()
        max_w = screen.size().width()*0.95
        max_h = screen.size().height()*0.95

        new_size = QSize(min(width, max_w), min(height, max_h))
        geo = QStyle.alignedRect(
            Qt.LeftToRight,
            Qt.AlignCenter,
            new_size,
            screen)
        self.setGeometry(geo)

    def _check_and_handle_win(self):
        """Checks for win and opens the appropriate screen if there is a winner.

        Returns:
            bool: is there a winner
        """
        if self.game.check_for_win() == PLAYER_SIDE:
            self._you_won()
        elif self.game.check_for_win() == OPPONENT_SIDE:
            self._game_over()
        else:
            return False
        return True

    def _opponent_fraction_curve(self, val):
        """Calculates the fraction based on the slider value

        Args:
            val (int): slider value

        Returns:
            float: the fraction
        """
        return round(tanh(val/10*pi), 2)

    # Page handlers: #--------------------------------------------------------#

    def _main_menu(self):
        """Opens the main menu
        """
        self.game_config = None

        self._update_opponent_display()
        self._clear_board_grid()
        self._switch_to_page(MAIN_MENU_PAGE_INDEX)
        self._center_and_set_res(500, 500)

    def _start_game(self, game_config=None):
        """Opens the game board configured with the game_config

        Args:
            game_config (GameConfig, optional): Game configuration
                for the game. If none data will be taken from the UI.
                Defaults to None.
        """
        if not game_config:
            game_config = self._get_game_conf_from_ui()

        self.game_config = game_config

        try:
            self.game = GaleShannonGame(self.game_config)
        except InvalidBoardSizeError as e:
            self._show_popup("Invalid board size", f"{e}")
            return
        except InvalidCoordinateError:
            self._show_popup(
                "Game config error",
                "There is a problem with the connections stored in the config")
            return
        except InvalidDirectionError:
            self._show_popup(
                "Game config error",
                "There is a problem with the connections stored in the config")
            return

        self._set_up_board_fields()
        self._update_side_display()
        self._set_up_time_display()

        # If opponent starts and is an agent, play their round:
        if self.game_config.who_starts == OPPONENT_SIDE and\
                self.game.opponent_is_agent():
            self.game.play_round(OPPONENT_SIDE)
            self._update_opponents_connection()

        aprox_h = self.game_config.n*70 + 200
        aprox_w = self.game_config.n*70 + 300

        self._center_and_set_res(aprox_w, aprox_h)
        self._switch_to_page(GAME_PAGE_INDEX)

    def _you_won(self):
        """When the player wins, this happens
        """
        self._switch_to_page(WIN_PAGE_INDEX)

    def _game_over(self):
        """When the player looses, this happens
        """
        self._switch_to_page(LOSS_PAGE_INDEX)

    def _load_game_state(self):
        """Opens a file dialog for loading game state
        """
        file_dialog = QFileDialog(self)
        file = file_dialog.getOpenFileName(
            caption="Choose game save file",
            filter="Json game saves ( *.json)"
        )[0]

        if not file:
            log.info("No file chosen")
            return

        with open(file, "r") as fs:
            try:
                game_config = read_game_config_from_json_stream(fs)
            except JSONDecodeError as e:
                self._show_popup(
                    "File error!",
                    f"JSON decoding error:\n{e}")
            except KeyError as e:
                self._show_popup(
                    "File error!",
                    f"Missing key {e}")
            except ValueError as e:
                self._show_popup(
                    "File error!",
                    f"Invalid data at key {e}")
            except Exception as e:
                self._show_popup(
                    "File error!", f"An error occured opening the file:\n{e}")
                return
            self._start_game(game_config)

    def _save_game_state(self):
        """Opens the file dialog for saving game state
        """
        file_dialog = QFileDialog(self)
        file = file_dialog.getSaveFileName(
            caption="Save game to file",
            filter="Json game saves ( *.json)"
        )[0]

        if not file:
            log.info("No file chosen")
            return

        if file.split('.')[-1] != "json":
            file += ".json"

        with open(file, "w") as fs:
            write_game_config_to_stream(fs, self.game.create_config())

    # Set ups/Updates: #------------------------------------------------------#

    def _stylize_ui(self):
        """Some stylesheets set on window init
        """
        self._ui.up_label.setStyleSheet(STATIC_V_QSS[OPPONENT_SIDE])
        self._ui.down_label.setStyleSheet(STATIC_V_QSS[OPPONENT_SIDE])
        self._ui.left_label.setStyleSheet(STATIC_V_QSS[PLAYER_SIDE])
        self._ui.right_label.setStyleSheet(STATIC_V_QSS[PLAYER_SIDE])

    def _set_up_board_fields(self):
        """Fills up the game board grid with BoardFieldDisplays
        """
        self._clear_board_grid()
        n = self.game_config.n
        self.board_field_displays = [list() for i in range(n)]

        for y in range(n):
            for x in range(n):
                board_field = self.game.get_board().get_board_field(x, y)
                if type(board_field) == StaticVertex:
                    bfd = StaticVDisplay(self, board_field, x, y)
                elif type(board_field) == Connector:
                    bfd = ConnectorDisplay(self, board_field, x, y)

                self.board_field_displays[x].append(bfd)

                # For some reason coordinates on grid_layout are swapped...
                self._ui.board_grid.addWidget(bfd, y, x)

    def _set_up_time_display(self):
        """Sets up the label displaying the game time
        """
        self._game_time = -1
        if self._clock is None:
            self._clock = QTimer()
            self._clock.setInterval(1000)
            self._clock.timeout.connect(self._update_time_display)
        else:
            self._clock.stop()
        self._clock.start()
        self._update_time_display()

    def _update_time_display(self):
        """Updates the label displaying play time
        """
        self._game_time += 1

        seconds = self._game_time % 60
        minutes = self._game_time//60

        self._ui.time_display.setText(f"{minutes:2}:{seconds:02}")

    def _clear_board_grid(self):
        """Deletes all boardfielddisplays in the grid layout on the screen
        """
        if self.board_field_displays:
            for row in self.board_field_displays:
                for label in row:
                    label.setParent(None)
                    label.deleteLater()
            self.board_field_displays = None

    def _update_opponent_display(self):
        """Nickname display in the main menu
        """
        opponent_code = self._ui.opponent_slider.value()
        if opponent_code == 11:
            name = "Another player"
        elif opponent_code == 0:
            name = "Pure randomness"
        elif 1 <= opponent_code < 3:
            name = "Very random"
        elif 3 <= opponent_code < 6:
            name = "Noticibly random"
        elif 6 <= opponent_code < 10:
            name = "Very easy"
        elif opponent_code == 10:
            name = "Full model, easy"

        if opponent_code <= 10:
            name += f" ({self._opponent_fraction_curve(opponent_code):.2f})"

        self._ui.opponent_display.setText(name)

    def _update_side_display(self):
        """Updates the display of the side that plays now
        """
        self._ui.side_indicator.setText(self.game.get_sides_nickname())

    def _update_opponents_connection(self):
        """Updates the connection made by the AI so that it is visible
        """
        x, y = self.game.get_opponent_last_move()
        self.board_field_displays[x][y].update()
        self.board_field_displays[x][y].disable()

    # Events: #---------------------------------------------------------------#

    def on_round_played(self, pos_x, pos_y):
        """Called when a round is played, triggered by player
        clicking the BoardFieldDisplay widget

        Args:
            pos_x (int): x position of the click
            pos_y (int): y position of the click
        """
        try:
            self.game.play_round(self.game.get_current_side(), (pos_x, pos_y))

            # Checking for a win
            if self._check_and_handle_win():
                return

            if self.game.opponent_is_agent() and\
                    self.game.get_current_side() == OPPONENT_SIDE:
                # If opponent is an agent capable of calculating the next move
                self.game.play_round(OPPONENT_SIDE)
                self._update_opponents_connection()
                if self._check_and_handle_win():
                    return

            self._update_side_display()
        except OccupiedConnectorError:
            self._show_popup(
                "Hey!", "This connection is occupied and you cannot take it!")


class BoardFieldDisplay(QLabel):
    """Represents an interactable widget on the game screen.
    """

    def __init__(self, parent, board_field, x_pos, y_pos):
        """Creates an instance of a modified label which acts as a button

        Args:
            parent (MainWindow): reference to the parent window
            board_field (BoardField): reference to the field this
                label represents
            x_pos (int): x position
            y_pos (int): y position
        """
        super().__init__()
        self._board_field = board_field
        self._x = x_pos
        self._y = y_pos
        self._parent = parent

        sizePolicy = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sizePolicy().hasHeightForWidth())
        self.setSizePolicy(sizePolicy)
        self.setMinimumSize(QSize(50, 50))
        self.setMaximumSize(QSize(50, 50))
        self.setAlignment(Qt.AlignCenter)

        self.update()

    def update(self):
        """Updates the contents of the label
        REMEMBER TO USE AFTER ALL DATA IS UPDATED
        """
        pass

    def mousePressEvent(self, ev):
        log.info(self._board_field.info())


class ConnectorDisplay(BoardFieldDisplay):
    """Represents a connector field in the game screen.
    """

    def __init__(self, parent, board_field, x_pos, y_pos):
        super().__init__(parent, board_field, x_pos, y_pos)
        self._enabled = True
        self.PRESSED_QSS = """
            font-size: 35px;
            border-radius: 0px;
            border: 5px solid gray;
            background: #222222;
            """

        self.update()

    def update(self):
        text = self._board_field.what_to_display()
        qss = CONNECTOR_QSS[self._board_field.get_side()]
        self.setText(text)
        self.setStyleSheet(qss)

    def disable(self):
        self._enabled = False

    def mousePressEvent(self, ev):
        super().mousePressEvent(ev)
        if self._enabled:
            self.setStyleSheet(self.PRESSED_QSS)

    def mouseReleaseEvent(self, ev):
        super().mouseReleaseEvent(ev)
        if self._enabled:
            log.info("Clicked on:\n"+self._board_field.info())
            self._parent.on_round_played(self._x, self._y)
            self.update()
        self.disable()


class StaticVDisplay(BoardFieldDisplay):
    """Represents a static vertex field in the game screen.
    """

    def __init__(self, parent, board_field, x_pos, y_pos):
        super().__init__(parent, board_field, x_pos, y_pos)

        text = self._board_field.what_to_display()
        side = self._board_field.get_side()
        qss = STATIC_V_QSS[side]
        self.setText(text)
        self.setStyleSheet(qss)

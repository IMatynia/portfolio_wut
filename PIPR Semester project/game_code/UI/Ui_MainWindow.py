# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'MainWindow.ui'
##
## Created by: Qt User Interface Compiler version 5.15.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(380, 381)
        MainWindow.setStyleSheet(u"QWidget\n"
"{\n"
"    color: #FFFFFF;\n"
"    background-color: #323232;\n"
"}\n"
"\n"
"QWidget:disabled\n"
"{\n"
"	color: #888888;\n"
"}")
        self.actionLoad_game = QAction(MainWindow)
        self.actionLoad_game.setObjectName(u"actionLoad_game")
        self.actionLoad_game.setEnabled(True)
        self.actionNew_game = QAction(MainWindow)
        self.actionNew_game.setObjectName(u"actionNew_game")
        self.actionNew_game.setEnabled(True)
        self.actionSave_game_state = QAction(MainWindow)
        self.actionSave_game_state.setObjectName(u"actionSave_game_state")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout_5 = QVBoxLayout(self.centralwidget)
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.stack = QStackedWidget(self.centralwidget)
        self.stack.setObjectName(u"stack")
        self.main_menu_page = QWidget()
        self.main_menu_page.setObjectName(u"main_menu_page")
        self.horizontalLayout_2 = QHBoxLayout(self.main_menu_page)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_3)

        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalSpacer_4 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_4)

        self.label_3 = QLabel(self.main_menu_page)
        self.label_3.setObjectName(u"label_3")
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)
        font = QFont()
        font.setFamily(u"Ubuntu Condensed")
        font.setPointSize(27)
        font.setBold(True)
        font.setItalic(False)
        font.setWeight(75)
        font.setStrikeOut(False)
        self.label_3.setFont(font)
        self.label_3.setStyleSheet(u"\n"
"color: qlineargradient(spread:pad, x1:0, y1:10, x2:200, y2:30, stop:0 rgba(255, 39, 111, 255), stop:0.328358 rgba(199, 44, 224, 255), stop:0.716418 rgba(93, 43, 227, 255), stop:1 rgba(36, 154, 255, 255));")
        self.label_3.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.label_3)

        self.label_5 = QLabel(self.main_menu_page)
        self.label_5.setObjectName(u"label_5")
        font1 = QFont()
        font1.setFamily(u"Ubuntu Thin")
        font1.setPointSize(15)
        font1.setItalic(True)
        self.label_5.setFont(font1)
        self.label_5.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.label_5)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.label_2 = QLabel(self.main_menu_page)
        self.label_2.setObjectName(u"label_2")
        sizePolicy1 = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy1)
        self.label_2.setAlignment(Qt.AlignCenter)

        self.horizontalLayout_3.addWidget(self.label_2)

        self.spinbox_n = QSpinBox(self.main_menu_page)
        self.spinbox_n.setObjectName(u"spinbox_n")
        sizePolicy2 = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.spinbox_n.sizePolicy().hasHeightForWidth())
        self.spinbox_n.setSizePolicy(sizePolicy2)
        self.spinbox_n.setMinimum(3)
        self.spinbox_n.setMaximum(1000)
        self.spinbox_n.setSingleStep(2)
        self.spinbox_n.setValue(5)

        self.horizontalLayout_3.addWidget(self.spinbox_n)


        self.verticalLayout.addLayout(self.horizontalLayout_3)

        self.line_2 = QFrame(self.main_menu_page)
        self.line_2.setObjectName(u"line_2")
        self.line_2.setFrameShape(QFrame.HLine)
        self.line_2.setFrameShadow(QFrame.Sunken)

        self.verticalLayout.addWidget(self.line_2)

        self.verticalLayout_3 = QVBoxLayout()
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.label_4 = QLabel(self.main_menu_page)
        self.label_4.setObjectName(u"label_4")
        sizePolicy1.setHeightForWidth(self.label_4.sizePolicy().hasHeightForWidth())
        self.label_4.setSizePolicy(sizePolicy1)
        self.label_4.setAlignment(Qt.AlignCenter)
        self.label_4.setWordWrap(True)

        self.verticalLayout_3.addWidget(self.label_4)

        self.opponent_slider = QSlider(self.main_menu_page)
        self.opponent_slider.setObjectName(u"opponent_slider")
        sizePolicy1.setHeightForWidth(self.opponent_slider.sizePolicy().hasHeightForWidth())
        self.opponent_slider.setSizePolicy(sizePolicy1)
        self.opponent_slider.setMaximum(11)
        self.opponent_slider.setPageStep(1)
        self.opponent_slider.setValue(10)
        self.opponent_slider.setTracking(True)
        self.opponent_slider.setOrientation(Qt.Horizontal)
        self.opponent_slider.setInvertedAppearance(False)
        self.opponent_slider.setInvertedControls(False)
        self.opponent_slider.setTickPosition(QSlider.TicksBelow)
        self.opponent_slider.setTickInterval(0)

        self.verticalLayout_3.addWidget(self.opponent_slider)

        self.opponent_display = QLabel(self.main_menu_page)
        self.opponent_display.setObjectName(u"opponent_display")
        sizePolicy1.setHeightForWidth(self.opponent_display.sizePolicy().hasHeightForWidth())
        self.opponent_display.setSizePolicy(sizePolicy1)
        self.opponent_display.setAlignment(Qt.AlignCenter)

        self.verticalLayout_3.addWidget(self.opponent_display)


        self.verticalLayout.addLayout(self.verticalLayout_3)

        self.line = QFrame(self.main_menu_page)
        self.line.setObjectName(u"line")
        self.line.setFrameShape(QFrame.HLine)
        self.line.setFrameShadow(QFrame.Sunken)

        self.verticalLayout.addWidget(self.line)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.player_starts = QRadioButton(self.main_menu_page)
        self.player_starts.setObjectName(u"player_starts")
        sizePolicy2.setHeightForWidth(self.player_starts.sizePolicy().hasHeightForWidth())
        self.player_starts.setSizePolicy(sizePolicy2)
        self.player_starts.setMinimumSize(QSize(0, 40))
        self.player_starts.setChecked(True)

        self.horizontalLayout.addWidget(self.player_starts)

        self.radioButton_2 = QRadioButton(self.main_menu_page)
        self.radioButton_2.setObjectName(u"radioButton_2")
        sizePolicy2.setHeightForWidth(self.radioButton_2.sizePolicy().hasHeightForWidth())
        self.radioButton_2.setSizePolicy(sizePolicy2)
        self.radioButton_2.setMinimumSize(QSize(0, 40))
        self.radioButton_2.setCheckable(True)

        self.horizontalLayout.addWidget(self.radioButton_2)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.play_game_button = QPushButton(self.main_menu_page)
        self.play_game_button.setObjectName(u"play_game_button")
        self.play_game_button.setEnabled(True)
        sizePolicy.setHeightForWidth(self.play_game_button.sizePolicy().hasHeightForWidth())
        self.play_game_button.setSizePolicy(sizePolicy)
        self.play_game_button.setMinimumSize(QSize(0, 50))

        self.verticalLayout.addWidget(self.play_game_button)

        self.verticalSpacer_5 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_5)


        self.horizontalLayout_2.addLayout(self.verticalLayout)

        self.horizontalSpacer_4 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_4)

        self.stack.addWidget(self.main_menu_page)
        self.game_page = QWidget()
        self.game_page.setObjectName(u"game_page")
        sizePolicy.setHeightForWidth(self.game_page.sizePolicy().hasHeightForWidth())
        self.game_page.setSizePolicy(sizePolicy)
        self.gridLayout = QGridLayout(self.game_page)
        self.gridLayout.setObjectName(u"gridLayout")
        self.scrollArea = QScrollArea(self.game_page)
        self.scrollArea.setObjectName(u"scrollArea")
        self.scrollArea.setWidgetResizable(True)
        self.scrollAreaWidgetContents = QWidget()
        self.scrollAreaWidgetContents.setObjectName(u"scrollAreaWidgetContents")
        self.scrollAreaWidgetContents.setGeometry(QRect(0, 0, 245, 311))
        self.horizontalLayout_4 = QHBoxLayout(self.scrollAreaWidgetContents)
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalSpacer_5 = QSpacerItem(0, 10, QSizePolicy.MinimumExpanding, QSizePolicy.Minimum)

        self.horizontalLayout_4.addItem(self.horizontalSpacer_5)

        self.verticalLayout_7 = QVBoxLayout()
        self.verticalLayout_7.setObjectName(u"verticalLayout_7")
        self.verticalSpacer_2 = QSpacerItem(20, 0, QSizePolicy.Minimum, QSizePolicy.MinimumExpanding)

        self.verticalLayout_7.addItem(self.verticalSpacer_2)

        self.gridLayout_2 = QGridLayout()
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.right_label = QLabel(self.scrollAreaWidgetContents)
        self.right_label.setObjectName(u"right_label")
        sizePolicy3 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Minimum)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.right_label.sizePolicy().hasHeightForWidth())
        self.right_label.setSizePolicy(sizePolicy3)
        self.right_label.setMinimumSize(QSize(50, 50))
        self.right_label.setAlignment(Qt.AlignCenter)

        self.gridLayout_2.addWidget(self.right_label, 1, 2, 1, 1)

        self.down_label = QLabel(self.scrollAreaWidgetContents)
        self.down_label.setObjectName(u"down_label")
        sizePolicy2.setHeightForWidth(self.down_label.sizePolicy().hasHeightForWidth())
        self.down_label.setSizePolicy(sizePolicy2)
        self.down_label.setMinimumSize(QSize(50, 50))
        self.down_label.setAlignment(Qt.AlignCenter)

        self.gridLayout_2.addWidget(self.down_label, 2, 1, 1, 1)

        self.board_grid = QGridLayout()
        self.board_grid.setObjectName(u"board_grid")

        self.gridLayout_2.addLayout(self.board_grid, 1, 1, 1, 1)

        self.left_label = QLabel(self.scrollAreaWidgetContents)
        self.left_label.setObjectName(u"left_label")
        sizePolicy3.setHeightForWidth(self.left_label.sizePolicy().hasHeightForWidth())
        self.left_label.setSizePolicy(sizePolicy3)
        self.left_label.setMinimumSize(QSize(50, 50))
        self.left_label.setAlignment(Qt.AlignCenter)

        self.gridLayout_2.addWidget(self.left_label, 1, 0, 1, 1)

        self.up_label = QLabel(self.scrollAreaWidgetContents)
        self.up_label.setObjectName(u"up_label")
        sizePolicy2.setHeightForWidth(self.up_label.sizePolicy().hasHeightForWidth())
        self.up_label.setSizePolicy(sizePolicy2)
        self.up_label.setMinimumSize(QSize(50, 50))
        self.up_label.setAlignment(Qt.AlignCenter)

        self.gridLayout_2.addWidget(self.up_label, 0, 1, 1, 1)


        self.verticalLayout_7.addLayout(self.gridLayout_2)

        self.verticalSpacer_3 = QSpacerItem(20, 0, QSizePolicy.Minimum, QSizePolicy.MinimumExpanding)

        self.verticalLayout_7.addItem(self.verticalSpacer_3)


        self.horizontalLayout_4.addLayout(self.verticalLayout_7)

        self.horizontalSpacer_2 = QSpacerItem(0, 10, QSizePolicy.MinimumExpanding, QSizePolicy.Minimum)

        self.horizontalLayout_4.addItem(self.horizontalSpacer_2)

        self.scrollArea.setWidget(self.scrollAreaWidgetContents)

        self.gridLayout.addWidget(self.scrollArea, 1, 0, 1, 1)

        self.verticalLayout_2 = QVBoxLayout()
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.verticalLayout_2.setSizeConstraint(QLayout.SetMinimumSize)
        self.label = QLabel(self.game_page)
        self.label.setObjectName(u"label")
        font2 = QFont()
        font2.setBold(True)
        font2.setWeight(75)
        self.label.setFont(font2)
        self.label.setAlignment(Qt.AlignCenter)

        self.verticalLayout_2.addWidget(self.label)

        self.horizontalSpacer = QSpacerItem(80, 20, QSizePolicy.Fixed, QSizePolicy.Minimum)

        self.verticalLayout_2.addItem(self.horizontalSpacer)

        self.time_display = QLabel(self.game_page)
        self.time_display.setObjectName(u"time_display")
        self.time_display.setAlignment(Qt.AlignCenter)

        self.verticalLayout_2.addWidget(self.time_display)

        self.line_4 = QFrame(self.game_page)
        self.line_4.setObjectName(u"line_4")
        self.line_4.setFrameShape(QFrame.HLine)
        self.line_4.setFrameShadow(QFrame.Sunken)

        self.verticalLayout_2.addWidget(self.line_4)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_2.addItem(self.verticalSpacer)

        self.label_6 = QLabel(self.game_page)
        self.label_6.setObjectName(u"label_6")
        self.label_6.setFont(font2)
        self.label_6.setAlignment(Qt.AlignCenter)

        self.verticalLayout_2.addWidget(self.label_6)

        self.side_indicator = QLabel(self.game_page)
        self.side_indicator.setObjectName(u"side_indicator")
        font3 = QFont()
        font3.setItalic(True)
        self.side_indicator.setFont(font3)
        self.side_indicator.setAlignment(Qt.AlignCenter)

        self.verticalLayout_2.addWidget(self.side_indicator)


        self.gridLayout.addLayout(self.verticalLayout_2, 1, 3, 1, 1)

        self.line_3 = QFrame(self.game_page)
        self.line_3.setObjectName(u"line_3")
        sizePolicy3.setHeightForWidth(self.line_3.sizePolicy().hasHeightForWidth())
        self.line_3.setSizePolicy(sizePolicy3)
        self.line_3.setFrameShape(QFrame.VLine)
        self.line_3.setFrameShadow(QFrame.Sunken)

        self.gridLayout.addWidget(self.line_3, 1, 2, 1, 1)

        self.stack.addWidget(self.game_page)
        self.win_page = QWidget()
        self.win_page.setObjectName(u"win_page")
        self.verticalLayout_4 = QVBoxLayout(self.win_page)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.label_8 = QLabel(self.win_page)
        self.label_8.setObjectName(u"label_8")
        font4 = QFont()
        font4.setPointSize(30)
        self.label_8.setFont(font4)
        self.label_8.setAlignment(Qt.AlignCenter)

        self.verticalLayout_4.addWidget(self.label_8)

        self.stack.addWidget(self.win_page)
        self.loose_page = QWidget()
        self.loose_page.setObjectName(u"loose_page")
        self.verticalLayout_6 = QVBoxLayout(self.loose_page)
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.label_9 = QLabel(self.loose_page)
        self.label_9.setObjectName(u"label_9")
        self.label_9.setFont(font4)
        self.label_9.setAlignment(Qt.AlignCenter)

        self.verticalLayout_6.addWidget(self.label_9)

        self.stack.addWidget(self.loose_page)

        self.verticalLayout_5.addWidget(self.stack)

        MainWindow.setCentralWidget(self.centralwidget)
        self.toolBar = QToolBar(MainWindow)
        self.toolBar.setObjectName(u"toolBar")
        MainWindow.addToolBar(Qt.TopToolBarArea, self.toolBar)
#if QT_CONFIG(shortcut)
        self.label_2.setBuddy(self.spinbox_n)
        self.label_4.setBuddy(self.opponent_slider)
#endif // QT_CONFIG(shortcut)

        self.toolBar.addAction(self.actionNew_game)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.actionLoad_game)
        self.toolBar.addAction(self.actionSave_game_state)
        self.toolBar.addSeparator()

        self.retranslateUi(MainWindow)

        self.stack.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"Shanon Switching game - Gale variant", None))
        self.actionLoad_game.setText(QCoreApplication.translate("MainWindow", u"Load game state", None))
        self.actionNew_game.setText(QCoreApplication.translate("MainWindow", u"New game", None))
        self.actionSave_game_state.setText(QCoreApplication.translate("MainWindow", u"Save game state", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"Shannon switching game", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"Gale variant", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"Board size", None))
        self.label_4.setText(QCoreApplication.translate("MainWindow", u"Opponent type", None))
        self.opponent_display.setText(QCoreApplication.translate("MainWindow", u"ai_display", None))
        self.player_starts.setText(QCoreApplication.translate("MainWindow", u"I start", None))
        self.radioButton_2.setText(QCoreApplication.translate("MainWindow", u"Opponent starts", None))
        self.play_game_button.setText(QCoreApplication.translate("MainWindow", u"Play the game", None))
        self.right_label.setText(QCoreApplication.translate("MainWindow", u"Right", None))
        self.down_label.setText(QCoreApplication.translate("MainWindow", u"Down", None))
        self.left_label.setText(QCoreApplication.translate("MainWindow", u"Left", None))
        self.up_label.setText(QCoreApplication.translate("MainWindow", u"Up", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"Time:", None))
        self.time_display.setText(QCoreApplication.translate("MainWindow", u"--:--", None))
        self.label_6.setText(QCoreApplication.translate("MainWindow", u"Turn:", None))
        self.side_indicator.setText(QCoreApplication.translate("MainWindow", u"plyr", None))
        self.label_8.setText(QCoreApplication.translate("MainWindow", u"You won! :D", None))
        self.label_9.setText(QCoreApplication.translate("MainWindow", u"Game over :(", None))
        self.toolBar.setWindowTitle(QCoreApplication.translate("MainWindow", u"toolBar", None))
    # retranslateUi


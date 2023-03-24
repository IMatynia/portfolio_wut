# SIDE CODES
PLAYER_SIDE = 1
OPPONENT_SIDE = -1
NO_SIDE = 0
VALID_SIDES = set([PLAYER_SIDE, OPPONENT_SIDE])

# DIRECTION CODES
HORIZONTAL = 1
VERTICAL = -1
NO_DIRECTION = 0
VALID_DIRECTIONS = set([HORIZONTAL, VERTICAL])

# UI PAGE INDEXES
MAIN_MENU_PAGE_INDEX = 0
GAME_PAGE_INDEX = 1
WIN_PAGE_INDEX = 2
LOSS_PAGE_INDEX = 3

# UI QSSes
STATIC_V_QSS = {
    PLAYER_SIDE: """
        border-radius: 3px;
        border: 3px solid #33EE44;
        background: #22DD33;
        """,
    OPPONENT_SIDE: """
        border-radius: 3px;
        border: 3px solid #EE3322;
        background: #DD2211;
        """
}

CONNECTOR_QSS = {
    NO_SIDE: """
        font-size: 35px;
        border-radius: 3px;
        border: 5px solid white;
        """,
    PLAYER_SIDE: """
        background: #22DD33;
        font-size: 35px;
        border-radius: 3px;
        border: 5px solid #33EE44;
        """,
    OPPONENT_SIDE: """
        background: #DD2211;
        font-size: 35px;
        border-radius: 3px;
        border: 5px solid #EE3322;
        """
}

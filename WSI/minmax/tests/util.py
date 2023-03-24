from game_board import BoardState


def place_move_array(board: BoardState, moves):
    for move in moves:
        x, y, player = move
        board.put_move(x, y, player)

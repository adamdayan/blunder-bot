import chess.pgn
import typing
import torch
import random
import math
from torch.utils.data import IterableDataset, get_worker_info

MOVE_HISTORY_LEN = 5

def result_to_value(result: str, turn: bool) -> int:
  if turn:
    if result == "1-0":
      y_value = 1
    elif result == "1/2-1/2":
      y_value = 0
    else:
      y_value = -1
  else:
    if result == "1-0":
      y_value = -1
    elif result == "1/2-1/2":
      y_value = 0
    else:
      y_value = 1
  return y_value

def squareset_to_tensor(ss: chess.SquareSet) -> torch.Tensor:
  bb = torch.zeros((1, 8, 8))
  for square in ss:
    bb[0, math.floor(square / 8), square%8] = 1
  return bb

def board_to_tensor(board: chess.Board, colour: bool) -> torch.Tensor:
  to_move_tensors = []
  opponent_tensors = []

  # flip the board if black is to play
  if not colour:
    board = board.mirror()
    colour = not colour

  for p in [chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN, chess.KING]:
    to_move_p = board.pieces(p, colour) 
    opponent_p = board.pieces(p, not colour)
    to_move_tensors.append(squareset_to_tensor(to_move_p))
    opponent_tensors.append(squareset_to_tensor(opponent_p))
  return torch.cat(to_move_tensors + opponent_tensors, dim=0)

def castling_rights_to_tensor(board: chess.Board) -> torch.Tensor:
  castling_tensors = []
  for colour in [board.turn, not board.turn]:
    if board.has_kingside_castling_rights(colour):
      castling_tensors.append(torch.ones(1, 8, 8))
    else:
      castling_tensors.append(torch.zeros(1, 8, 8))

    if board.has_queenside_castling_rights(colour):
      castling_tensors.append(torch.ones(1, 8, 8))
    else:
      castling_tensors.append(torch.zeros(1, 8, 8))
  
  return torch.cat(castling_tensors, dim=0)

def move_to_tensor(move: chess.Move, colour: bool):
    # network views all moves as coming from white's perspective
    if not colour:
        from_square = chess.square_mirror(move.from_square)
        to_square = chess.square_mirror(move.to_square)
    else:
        from_square = move.from_square
        to_square = move.to_square

    if move.promotion is None:
        move_idx = (from_square * 64) + to_square
    else:
        # encode promotions based on side of board, from, to and promoted piece type
        # (2 * 8 * 8 * 4)
        if from_square >= 48 and from_square < 57:
            rebased_from = from_square - 48
            rebased_to = to_square - 57
            side = 0
        else: # NOTE: this is not needed now because promotion moves will only happen at the top of the board
            rebased_from = from_square - 8
            rebased_to = to_square
            side = 1

        move_idx = (
            (side * 8 * 8 * 4)
            + (rebased_from * 8 * 4)
            + (rebased_to * 4)
            + (move.promotion - 1)
            + 4095
        )

    return torch.tensor([move_idx])

def game_to_input(game):
    game_len = game.end().ply()
    move_idx = random.randint(0, game_len -1)

    # iterate forward to chosen move
    move_node = game
    colour = move_node.turn()
    for _ in range(move_idx):
      move_node = move_node.next()
      colour = move_node.turn()

    all_tensors = []
    # add move and history
    all_tensors.append(board_to_tensor(move_node.board(), colour))
    history_node = move_node
    last_valid_pos = all_tensors[0] 
    for i in range(1, MOVE_HISTORY_LEN):
      if move_idx - i >= 0:
        history_node = history_node.parent
        last_valid_pos = board_to_tensor(history_node.board(), colour)
        all_tensors.append(last_valid_pos)
      else:
        # if move is too early to have full history then just add enough zeros to make dims match
        for _ in range(MOVE_HISTORY_LEN - i):
          all_tensors.append(last_valid_pos)  
        break
    
    # add castling rights
    all_tensors.append(castling_rights_to_tensor(move_node.board()))

      # add side to move
    if game.turn():
      all_tensors.append(torch.ones(1, 8, 8))
    else:
      all_tensors.append(torch.zeros(1, 8, 8))

      # Leela Chess Zero claims this helps convolutions detect edge of board
    all_tensors.append(torch.ones(1, 8, 8))

    x = torch.cat(all_tensors, dim=0)
    y_policy = move_to_tensor(move_node.next().move, colour)
    y_value = result_to_value(game.headers["Result"], move_node.turn())
    return (x, y_policy, y_value)

class LichessDataset(IterableDataset):
  def __init__(self, pgn_paths: list[str]):
    self.pgn_paths = pgn_paths

  def __iter__(self):
    worker_info = get_worker_info() 
    # if we're only using 1 process then use all paths
    if worker_info is None:
      for pgn_path in self.pgn_paths:
        pgn = open(pgn_path)
        while ((game := chess.pgn.read_game(pgn)) is not None):
          if (game.end().ply() <= 2):
            continue
          else:
            yield game_to_input(game)
    else:
      worker_id = worker_info.id
      # otherwise give 1 pgn file to each worker
      pgn = open(self.pgn_paths[worker_id])
      while ((game := chess.pgn.read_game(pgn)) is not None):
        if (game.end().ply() <= 2):
          continue
        else:
          yield game_to_input(game)
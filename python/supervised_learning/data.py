import os
import chess.pgn
import typing
import torch
import random
import math
from torch.utils.data import Dataset, DataLoader

MOVE_HISTORY_LEN = 5

def squareset_to_tensor(ss: chess.SquareSet) -> torch.Tensor:
  bb = torch.zeros((1, 8, 8))
  for square in ss:
    bb[0, math.floor(square / 8), square%8] = 1
  return bb

def board_to_tensor(board: chess.Board) -> torch.Tensor:
  to_move_tensors = []
  opponent_tensors = []
  for p in [chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN, chess.KING]:
    to_move_p = board.pieces(p, board.turn)
    opponent_p = board.pieces(p, not board.turn)
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

def move_to_tensor(move: chess.Move):
  move_matrix = torch.zeros(64, 64)
  move_matrix[move.from_square][move.to_square] = 1
  return move_matrix

class LichessDataset(Dataset):
  def __init__(self, pgn_paths: list[str]):
    self.games = []
    for path in pgn_paths:
      self.read_pgn(path) # this is super memory inefficient, this is just to get started

  def read_pgn(self, pgn_path: str):
    pgn = open(pgn_path)
    i = 0
    while ((game := chess.pgn.read_game(pgn)) is not None and i < 100):
      self.games.append(game)
      i += 1
  
  def __len__(self):
    return len(self.games)

  def __getitem__(self, idx: int):
    game = self.games[idx]  
    game_len = game.end().ply()
    move_idx = random.randint(0, game_len -1)

    # iterate forward to chosen move
    move_node = game
    for _ in range(move_idx):
      move_node = move_node.next()

    all_tensors = []
    # add move and history
    all_tensors.append(board_to_tensor(move_node.board()))
    history_node = move_node
    for i in range(1, MOVE_HISTORY_LEN):
      if move_idx - i >= 0:
        all_tensors.append(board_to_tensor(history_node.parent.board()))
      else:
        # if move is too early to have full history then just add enough zeros to make dims match
        for _ in range(MOVE_HISTORY_LEN - i):
          all_tensors.append(torch.zeros(12, 8, 8))  
        break
    
    # add castling rights
    all_tensors.append(castling_rights_to_tensor(move_node.board()))

    # Leela Chess Zero claims this helps convolutions detect edge of board
    all_tensors.append(torch.ones(1, 8, 8))

    x = torch.cat(all_tensors, dim=0)
    y_policy = move_to_tensor(move_node.next().move)
    if move_node.turn:
      if game.headers["Result"] == "1-0":
        y_value = 1
      else:
        y_value = -1
    elif not move_node.turn:
      if game.headers["Result"] == "1-0":
        y_value = -1
      else:
        y_value = 1
    return {"x": x, "y_policy": y_policy, "y_value": y_value}
 
    
    
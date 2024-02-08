import os
import chess.pgn
import typing
import torch
import random
import math
from torch.utils.data import Dataset

MOVE_HISTORY_LEN = 5

def squareset_to_tensor(ss: chess.SquareSet) -> torch.Tensor:
  bb = torch.zeros((1, 8, 8))
  for square in ss:
    bb[0, math.floor(square / 8), square%8] = 1
  return bb

def board_to_tensor(board: chess.Board, colour: bool) -> torch.Tensor:
  to_move_tensors = []
  opponent_tensors = []

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

def move_to_tensor(move: chess.Move):
  if move.promotion is None:
    move_idx = (move.from_square * 64) + move.to_square
  else:
    # encode promotions based on side of board, from, to and promoted piece type 
    # (2 * 8 * 8 * 4)
    if (move.from_square >= 48 and move.from_square < 57):
      rebased_from = move.from_square - 48
      rebased_to = move.to_square - 57
      side = 0 
    else:
      rebased_from = move.from_square + 8
      rebased_to = move.to_square
      side = 1

    move_idx = (side * 8 * 8 *4) + (rebased_from * 8 * 4) + (rebased_to * 4) + (move.promotion - 1) + 4095
  return torch.tensor([move_idx])

class LichessDataset(Dataset):
  def __init__(self, pgn_paths: list[str]):
    self.games = []
    for path in pgn_paths:
      self.read_pgn(path) # this is super memory inefficient, this is just to get started

  def read_pgn(self, pgn_path: str):
    pgn = open(pgn_path)
    i = 0
    while ((game := chess.pgn.read_game(pgn)) is not None):
      if (game.end().ply() > 2):
        self.games.append(game)
      # TODO: just for local testing
      i+=1
      if i > 200:
        break
  
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
    colour = move_node.turn()

    all_tensors = []
    # add move and history
    all_tensors.append(board_to_tensor(move_node.board(), colour))
    history_node = move_node
    for i in range(1, MOVE_HISTORY_LEN):
      if move_idx - i >= 0:
        all_tensors.append(board_to_tensor(history_node.parent.board(), colour))
      else:
        # if move is too early to have full history then just add enough zeros to make dims match
        for _ in range(MOVE_HISTORY_LEN - i):
          all_tensors.append(torch.zeros(12, 8, 8))  
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
    return (x, y_policy, y_value)
 
    
    
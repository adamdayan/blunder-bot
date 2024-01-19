#include "move_generator.h"
#include "constants.h"



// generates all rays from a given square
BitBoard generateRay(int start_rank, int start_file, Direction dir) {
  BitBoard bb;
  // NOTE: would it be better to split these into separate functions?
  // Also, is there some better bit-shifty way to do this?
  switch (dir) {
    case Direction::North: {
      for (int rank = start_rank; rank < 8; rank++) {
        bb.setBit(rank, start_file);
      }
      break;
    }
    case Direction::NorthEast: {
      for (int rank = start_rank; rank < 8; rank++) {
        for (int file = start_file; file < 8; file++) {
          bb.setBit(rank, file);
        }
      }
      break;
    }
    case Direction::East : {
      for (int file = start_file; file < 8; file++) {
        bb.setBit(start_rank, file);
      }
      break;
    }
    case Direction::SouthEast : {
      for (int rank = start_rank; rank >= 0; rank--) {
        for (int file = start_file; file < 8; file++) {
          bb.setBit(rank, file);
        }
      }
      break;
    } 
    case Direction::South : {
      for (int rank = start_rank; rank >= 0; rank--) {
        bb.setBit(rank, start_file);
      }
      break;
    }
    case Direction::SouthWest : {
      for (int rank = start_rank; rank >= 0; rank--) {
        for (int file = start_file; file >= 0; file--) {
          bb.setBit(rank, file);
        }
        break;
      }
    }
    case Direction::West : {
      for (int file = start_file; file >= 0; file--) {
        bb.setBit(start_rank, file);
      }
      break;
    }
    case Direction::NorthWest : {
      for (int rank = start_rank; rank < 8; rank++) {
        for (int file = start_file; file >= 0; file--) {
          bb.setBit(rank, file);
        }
      }
      break;
    }
  }
  return bb;  
}

// generates all possible knight moves from a given square
BitBoard generateKnightMoves(int rank, int file) {
  BitBoard bb;
  std::array<int, 4> deltas{6, 15, 17, 10};

  int index = rankFileToIndex(rank, file);
  for (const auto& delta : deltas) {
    if (index + delta <= 63) {
      bb.setBit(index + delta);
    }
    if (index - delta >= 0) {
      bb.setBit(index - delta);
    }
  }

  return bb;
}

BitBoard computePawnMoves(const Position& pos) {
  
}

BitBoard MoveGenerator::computeQuietPawnPushes(const Position& pos, const BoardPerspective& persp) {
  BitBoard single_pushes = computeSinglePawnPushes(pos, persp);
  BitBoard double_pushes = computeDoublePawnPushes(pos, persp, single_pushes);
}

BitBoard MoveGenerator::computeSinglePawnPushes(const Position& pos, const BoardPerspective& persp) {
  BitBoard pawns = pos.getPieceBitBoard(persp.side_to_move, PieceType::Pawn);
  // compute single pawn pushes
  // pawns about to promote treated separately 
  pawns.board &= ~persp.pawn_pre_promote_rank;
  // shift the pawns up 1 rank 
  pawns = pawns.shift(persp.up);
  // check there are no blocking piece
  pawns.board &= ~pos.getAllPiecesBitBoard().board;
  return pawns;
}

BitBoard MoveGenerator::computeDoublePawnPushes(const Position& pos, const BoardPerspective& persp, BitBoard single_pushes) {
  // get possible single pushes because if you can't push 1 rank you also can't push 2
  single_pushes.board &= persp.double_push_possible;
  // shift the pawns up 1 rank 
  BitBoard double_push = single_pushes.shift(persp.up);
  // check there are no blocking piece
  double_push.board &= ~pos.getAllPiecesBitBoard().board;
  return double_push;
}

BitBoard MoveGenerator::computePawnCaptures(const Position& pos, const BoardPerspective& persp) {
  BitBoard pawns = pos.getPieceBitBoard(persp.side_to_move, PieceType::Pawn);
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  BitBoard enemy_king = pos.getPieceBitBoard(persp.opponent, PieceType::King);
  BitBoard takeable_pieces = enemy_pieces & ~enemy_king;
  
  // get up-west captures
  BitBoard west_cap = pawns.shift(persp.up_west);
  west_cap &= takeable_pieces;

  BitBoard east_cap = pawns.shift(persp.up_east);
  east_cap &= takeable_pieces;

  // TODO: can't return both bitboards from here, need to extract moves. 
  // MUST DECIDE HOW MOVES ARE EXTRACTED AND PASSED AROUND
  return east_cap;
}

std::vector<Move> MoveGenerator::generateMoves(const Position& pos) {
  Direction up, down;
  // directions switch depending on side to move
  BoardPerspective persp(pos.getSideToMove());

}

MoveGenerator::MoveGenerator() {
  // precomputes all rays and knight moves
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      for (int dir = Direction::North; dir <= Direction::NorthWest; dir++) {
        rays[rank][file][dir] = generateRay(rank, file, static_cast<Direction>(dir));
      }
      knight_moves[rank][file] = generateKnightMoves(rank, file);
    }
  }
}
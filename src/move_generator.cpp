#include "move_generator.h"
#include "constants.h"
#include "position.h"
#include "utils.h"



// generates all rays from a given square
BitBoard precomputeRay(int start_rank, int start_file, Direction dir) {
  BitBoard bb;
  // NOTE: would it be better to split these into separate functions?
  // Also, is there some better bit-shifty way to do this?
  switch (dir) {
    case Direction::North: {
      for (int rank = start_rank + 1; rank < 8; rank++) {
        bb.setBit(rank, start_file);
      }
      break;
    }
    case Direction::NorthEast: {
      int file = start_file;
      for (int rank = start_rank + 1; rank < 8; rank++) {
        file++;
        if (file < 8) {
          bb.setBit(rank, file);
        }
      }
      break;
    }
    case Direction::East : {
      for (int file = start_file + 1; file < 8; file++) {
        bb.setBit(start_rank, file);
      }
      break;
    }
    case Direction::SouthEast : {
      int file = start_file;
      for (int rank = start_rank - 1; rank >= 0; rank--) {
        file++;
        if (file < 8) {
          bb.setBit(rank, file);
        }
      }
      break;
    } 
    case Direction::South : {
      for (int rank = start_rank - 1; rank >= 0; rank--) {
        bb.setBit(rank, start_file);
      }
      break;
    }
    case Direction::SouthWest : {
      int file = start_file;
      for (int rank = start_rank - 1; rank >= 0; rank--) {
        file--;
        if (file >= 0) {
          bb.setBit(rank, file);
        }
      }
      break;
    }
    case Direction::West : {
      for (int file = start_file - 1; file >= 0; file--) {
        bb.setBit(start_rank, file);
      }
      break;
    }
    case Direction::NorthWest : {
      int file = start_file;
      for (int rank = start_rank + 1; rank < 8; rank++) {
        file--;
        if (file >= 0) {
          bb.setBit(rank, file);
        }
      }
      break;
    }
  }
  return bb;  
}

// generates all possible knight moves from a given square
BitBoard precomputeKnightMoves(int rank, int file) {
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

// generates all king moves from a given square
BitBoard precomputeKingMoves(int rank, int file) {
  BitBoard bb;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      // king can't stay in the same place
      if (i == 0 && j == 0) {
        continue;
      }
      
      int dest_rank = rank + i;
      int dest_file = file + j;
      if (dest_rank >= 0 && dest_rank < 8 && dest_file >= 0 && dest_file < 8) {
        bb.setBit(rank + i, file + j);
      }
    }
  }
  return bb;
}

void MoveGenerator::extractPawnMoves(BitBoard bb, int offset, MoveType type, MoveVec& moves, PieceType promotion) {
  while (!bb.isEmpty()) {
    int dest = bb.popHighestSetBit();
    // NOTE: offset sign changes per side
    moves.emplace_back(dest - offset, dest, type, promotion);
  }
}

void MoveGenerator::extractPieceMoves(BitBoard bb, int source, MoveType type, MoveVec& moves) {
  while (!bb.isEmpty()) {
    int dest = bb.popHighestSetBit();
    moves.emplace_back(source, dest, type);
  }
}

void MoveGenerator::generatePawnMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  generateQuietPawnPushes(pos, persp, moves);
  generatePawnCaptures(pos, persp, moves);
  generatePromotions(pos, persp, moves);
  generateEnPassant(pos, persp, moves);
}
  
void MoveGenerator::generateQuietPawnPushes(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard single_pushes = computeSinglePawnPushes(pos, persp);
  extractPawnMoves(single_pushes, SINGLE_PAWN_PUSH_OFFSET * persp.offset_sign, MoveType::Quiet,
                   moves);

  BitBoard double_pushes = computeDoublePawnPushes(pos, persp, single_pushes);
  extractPawnMoves(double_pushes, DOUBLE_PAWN_PUSH_OFFSET * persp.offset_sign, MoveType::Quiet,
                   moves);
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

void MoveGenerator::generatePawnCaptures(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard pawns = pos.getPieceBitBoard(persp.side_to_move, PieceType::Pawn);
  // we will handle pawns about to promote seperately in generatePromotions()
  pawns &= ~persp.pawn_pre_promote_rank;
  // get all takeable pieces
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  BitBoard enemy_king = pos.getPieceBitBoard(persp.opponent, PieceType::King);
  BitBoard takeable_pieces = enemy_pieces & ~enemy_king;
  
  // get up-west captures
  BitBoard west_cap = pawns.shift(persp.up_west);
  west_cap &= takeable_pieces;
  extractPawnMoves(west_cap, PAWN_WEST_CAPTURE_OFFSET, MoveType::Capture, moves);

  BitBoard east_cap = pawns.shift(persp.up_east);
  east_cap &= takeable_pieces;
  extractPawnMoves(east_cap, PAWN_EAST_CAPTURE_OFFSET, MoveType::Capture, moves);
}

void MoveGenerator::generatePromotions(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard pawns = pos.getPieceBitBoard(persp.side_to_move, PieceType::Pawn);
  // only care about pawns about to promote
  pawns &= persp.pawn_pre_promote_rank;

  // get all takeable pieces
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  BitBoard enemy_king = pos.getPieceBitBoard(persp.opponent, PieceType::King);
  BitBoard takeable_pieces = enemy_pieces & ~enemy_king;
  
  // get captures resulting in promotion
  BitBoard west_cap = pawns.shift(persp.up_west);
  west_cap &= takeable_pieces;
  BitBoard east_cap = pawns.shift(persp.up_east);
  east_cap &= takeable_pieces;
  

  // get single pushes resulting in promotion
  BitBoard single_push = pawns.shift(persp.up);
  // check there are no blocking piece
  single_push &= ~pos.getAllPiecesBitBoard();

  for (int piece = PieceType::Knight; piece < PieceType::King; piece++) {
    extractPawnMoves(west_cap, persp.offset_sign * PAWN_WEST_CAPTURE_OFFSET, MoveType::Capture, moves, static_cast<PieceType>(piece));
    extractPawnMoves(east_cap, persp.offset_sign * PAWN_EAST_CAPTURE_OFFSET, MoveType::Capture, moves, static_cast<PieceType>(piece));
    extractPawnMoves(single_push, persp.offset_sign * SINGLE_PAWN_PUSH_OFFSET, MoveType::Quiet, moves, static_cast<PieceType>(piece));
  }
}

void MoveGenerator::generateEnPassant(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard pawns = pos.getPieceBitBoard(persp.side_to_move, PieceType::Pawn);
  // TODO: work out how to set and clear enpassant bitboard in Position.
  // Currently only set by FEN parsing
  BitBoard enpassant_targets = pos.getEnpassantBitBoard();

  // no possible en passant captures
  if (pawns.isEmpty() || enpassant_targets.isEmpty()) {
    return;
  }

  // compute possible en passant captures
  BitBoard west_cap = pawns.shift(persp.up_west);
  west_cap &= enpassant_targets;
  extractPawnMoves(west_cap, persp.offset_sign * PAWN_WEST_CAPTURE_OFFSET, MoveType::EnPassantCapture, moves);

  BitBoard east_cap = pawns.shift(persp.up_east);
  east_cap &= enpassant_targets;
  extractPawnMoves(east_cap, persp.offset_sign * PAWN_EAST_CAPTURE_OFFSET, MoveType::EnPassantCapture, moves);
} 

void MoveGenerator::generateKnightMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard knights = pos.getPieceBitBoard(persp.side_to_move, PieceType::Knight);
  while (!knights.isEmpty()) {
    int knight_index = knights.popHighestSetBit();
    BitBoard possible_hops = knight_moves[indexToRank(knight_index)][indexToFile(knight_index)];

    BitBoard quiet_hops = possible_hops & ~pos.getAllPiecesBitBoard();
    extractPieceMoves(quiet_hops, knight_index, MoveType::Quiet, moves);

    BitBoard capture_hops = possible_hops & pos.getPieceBitBoard(persp.opponent, PieceType::All);
    extractPieceMoves(capture_hops, knight_index, MoveType::Capture, moves);
  }
}

void MoveGenerator::generateBishopMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard bishops = pos.getPieceBitBoard(persp.side_to_move, PieceType::Bishop);
  while (!bishops.isEmpty()) {
    int bishop_index = bishops.popHighestSetBit();
    int bishop_rank = indexToRank(bishop_index);
    int bishop_file = indexToFile(bishop_index);

    // north-east
    BitBoard ne_ray = rays[bishop_rank][bishop_file][Direction::NorthEast];
    generateBishopRayMoves(pos, persp, moves, ne_ray, Direction::NorthEast, bishop_index);

    // south-east
    BitBoard se_ray = rays[bishop_rank][bishop_file][Direction::SouthEast];
    generateBishopRayMoves(pos, persp, moves, se_ray, Direction::SouthEast, bishop_index);

    // south-west
    BitBoard sw_ray = rays[bishop_rank][bishop_file][Direction::SouthWest];
    generateBishopRayMoves(pos, persp, moves, sw_ray, Direction::SouthWest, bishop_index);

    // north-west
    BitBoard nw_ray = rays[bishop_rank][bishop_file][Direction::NorthWest];
    generateBishopRayMoves(pos, persp, moves, nw_ray, Direction::NorthWest, bishop_index);
  }
}

void MoveGenerator::generateBishopRayMoves(const Position &pos,
                                           const BoardPerspective &persp,
                                           MoveVec &moves, const BitBoard &ray,
                                           Direction dir, int bishop_index) {
  BitBoard all_pieces = pos.getAllPiecesBitBoard();
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  BitBoard intersect = ray & all_pieces;
  int blocking_index;
  int offset;
  if (dir == Direction::NorthEast) {
    blocking_index = intersect.getLowestSetBit();
    offset = 9;
  } else if (dir == Direction::SouthEast) {
    blocking_index = intersect.getHighestSetBit();
    offset = -7;
  } else if (dir == Direction::SouthWest) {
    blocking_index = intersect.getHighestSetBit();
    offset = -9;
  } else if (dir == Direction::NorthWest) {
    blocking_index = intersect.getLowestSetBit();
    offset = 7;
  }
  // NOTE: consider how to error handle bad cases - probably unnecessary because we are far from user input or syscalls

  int b_index = bishop_index + offset;
  // travel along the ray to the blocking_index if there is one otherwise to the
  // edge of the board
  while (b_index != blocking_index && indexToFile(b_index) >= 0 &&
         indexToFile(b_index) < 8 && indexToRank(b_index) >= 0 &&
         indexToRank(b_index) < 8) {
    b_index += offset;
    moves.emplace_back(bishop_index, b_index, MoveType::Quiet);
  }
  // if the blocking piece is an enemy piece then we have a capture move
  if (blocking_index >= 0 && enemy_pieces.getBit(blocking_index)) {
    moves.emplace_back(bishop_index, blocking_index, MoveType::Capture);
  }
}

void MoveGenerator::generateRookMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard rooks = pos.getPieceBitBoard(persp.side_to_move, PieceType::Rook);
  while (!rooks.isEmpty()) {
    int rook_index = rooks.popHighestSetBit();
    int rook_rank = indexToRank(rook_index);
    int rook_file = indexToFile(rook_index);

    // north
    BitBoard n_ray = rays[rook_rank][rook_file][Direction::North];
    generateRookRayMoves(pos, persp, moves, n_ray, Direction::North, rook_index);

    // east
    BitBoard e_ray = rays[rook_rank][rook_file][Direction::East];
    generateRookRayMoves(pos, persp, moves, e_ray, Direction::East, rook_index);

    // south 
    BitBoard s_ray = rays[rook_rank][rook_file][Direction::South];
    generateRookRayMoves(pos, persp, moves, s_ray, Direction::South, rook_index);

    // west 
    BitBoard w_ray = rays[rook_rank][rook_file][Direction::West];
    generateRookRayMoves(pos, persp, moves, w_ray, Direction::West, rook_index);
  }
}

void MoveGenerator::generateRookRayMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves, const BitBoard& ray, Direction dir, int rook_index) {
  BitBoard all_pieces = pos.getAllPiecesBitBoard();
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  int blocking_index;
  int offset;
  BitBoard intersect = all_pieces & ray;
  if (dir == Direction::North) {
    blocking_index = intersect.getLowestSetBit();
    offset = 8;
  } else if (dir == Direction::East) {
    blocking_index = intersect.getLowestSetBit();
    offset = 1;
  } else if (dir == Direction::South) {
    blocking_index = intersect.getHighestSetBit();
    offset = -8;
  } else if (dir == Direction::West) {
    blocking_index = intersect.getLowestSetBit();
    offset = -1;
  }
  int r_index = rook_index + offset;
  // travel along ray to blocking index if there is one or to edge of the board
  while (r_index != blocking_index && indexToRank(r_index) >= 0 &&
         indexToRank(r_index) < 8 && indexToFile(r_index) >= 0 &&
         indexToFile(r_index) < 8) {
    r_index += offset;
    moves.emplace_back(rook_index, r_index, MoveType::Quiet);
  }
  // if the blocking piece is an enemy piece then we have a capture move
  if (blocking_index >= 0 && enemy_pieces.getBit(blocking_index)) {
    moves.emplace_back(rook_index, blocking_index, MoveType::Capture);
  }
}

void MoveGenerator::generateQueenMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard queens = pos.getPieceBitBoard(persp.side_to_move, PieceType::Queen);
  while (!queens.isEmpty()) {
    int queen_index = queens.popHighestSetBit();
    int queen_rank = indexToRank(queen_index);
    int queen_file = indexToFile(queen_index);

    // queen is a combo of a bishop and rook so we can use their move generation functions

    // north
    BitBoard n_ray = rays[queen_rank][queen_file][Direction::North];
    generateRookRayMoves(pos, persp, moves, n_ray, Direction::North, queen_index);

    // east
    BitBoard e_ray = rays[queen_rank][queen_file][Direction::East];
    generateRookRayMoves(pos, persp, moves, e_ray, Direction::East, queen_index);

    // south 
    BitBoard s_ray = rays[queen_rank][queen_file][Direction::South];
    generateRookRayMoves(pos, persp, moves, s_ray, Direction::South, queen_index);

    // west 
    BitBoard w_ray = rays[queen_rank][queen_file][Direction::West];
    generateRookRayMoves(pos, persp, moves, w_ray, Direction::West, queen_index);

    // north-east
    BitBoard ne_ray = rays[queen_rank][queen_file][Direction::NorthEast];
    generateBishopRayMoves(pos, persp, moves, ne_ray, Direction::NorthEast, queen_index);

    // south-east
    BitBoard se_ray = rays[queen_rank][queen_file][Direction::SouthEast];
    generateBishopRayMoves(pos, persp, moves, se_ray, Direction::SouthEast, queen_index);

    // south-west
    BitBoard sw_ray = rays[queen_rank][queen_file][Direction::SouthWest];
    generateBishopRayMoves(pos, persp, moves, sw_ray, Direction::SouthWest, queen_index);

    // north-west
    BitBoard nw_ray = rays[queen_rank][queen_file][Direction::NorthWest];
    generateBishopRayMoves(pos, persp, moves, nw_ray, Direction::NorthWest, queen_index);
  }
}

void MoveGenerator::generateKingMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard king = pos.getPieceBitBoard(persp.side_to_move, PieceType::King);
  BitBoard all_pieces = pos.getAllPiecesBitBoard();
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  int king_index = king.getHighestSetBit();
  BitBoard possible_king_moves = king_moves[indexToRank(king_index)][indexToFile(king_index)];

  BitBoard quiet_moves = possible_king_moves & ~all_pieces;
  extractPieceMoves(quiet_moves, king_index, MoveType::Quiet, moves);

  BitBoard captures = possible_king_moves & enemy_pieces;
  extractPieceMoves(captures, king_index, MoveType::Capture, moves);
}

MoveVec MoveGenerator::generateMoves(const Position& pos) {
  // directions switch depending on side to move
  BoardPerspective persp(pos.getSideToMove());

}

MoveGenerator::MoveGenerator() {
  // precomputes all rays, knight moves and king moves
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      for (int dir = Direction::North; dir <= Direction::NorthWest; dir++) {
        rays[rank][file][dir] = precomputeRay(rank, file, static_cast<Direction>(dir));
      }
      knight_moves[rank][file] = precomputeKnightMoves(rank, file);
      king_moves[rank][file] = precomputeKingMoves(rank, file);
    }
  }
}
#include <algorithm>

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
  if (rank + 2 < 8 && file + 1 < 8) {
    bb.setBit(rank + 2, file + 1);
  }
  if (rank + 2 < 8 && file - 1 >= 0) {
    bb.setBit(rank + 2, file - 1);
  }

  if (rank + 1 < 8 && file + 2 < 8) {
    bb.setBit(rank + 1, file + 2);
  }
  if (rank + 1 < 8 && file - 2 >= 0) {
    bb.setBit(rank + 1, file - 2);
  }

  if (rank - 2 >= 0 && file + 1 < 8) {
    bb.setBit(rank - 2, file + 1);
  }
  if (rank - 2 >= 0 && file - 1 >= 0) {
    bb.setBit(rank - 2, file - 1);
  }

  if (rank - 1 >= 0 && file + 2 < 8) {
    bb.setBit(rank - 1, file + 2);
  }
  if (rank - 1 >= 0 && file - 2 >= 0) {
    bb.setBit(rank - 1, file - 2);
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

// TODO: test this
// returns a bitboard of the ray between square 1 and square 2 inclusive. if
// there is no such ray then returns an empty bitboard
BitBoard precomputeRaysBetween(int s1_index, int s2_index, const BoardMatrix<std::array<BitBoard, 8>>& rays) {
  int s1_rank = indexToRank(s1_index);
  int s1_file = indexToFile(s1_index);

  for (const auto& ray : rays[s1_rank][s1_file]) {
    if (ray.getBit(s2_index)) {
      BitBoard rel_ray = ray;
      rel_ray.setBit(s1_index);
      if (s1_index < s2_index) {
        rel_ray.clearBitsAbove(s2_index + 1);
        rel_ray.clearBitsBelow(s1_index - 1);
      } else {
        rel_ray.clearBitsAbove(s1_index + 1);
        rel_ray.clearBitsBelow(s2_index - 1);
      }
      return rel_ray;
    }
  }
  return BitBoard();
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

BitBoard MoveGenerator::getRayBetween(int s1_index, int s2_index) {
  return rays_between[indexToRank(s1_index)][indexToFile(s1_index)][indexToRank(s2_index)][indexToFile(s2_index)];
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

BitBoard MoveGenerator::computeWestPawnCaptures(const Position& pos, const BoardPerspective& persp, const BitBoard& relevant_pawns, const BitBoard& takeable_pieces) {
  // get up-west captures
  BitBoard west_cap = relevant_pawns.shift(persp.up_west);
  west_cap &= takeable_pieces;
  return west_cap;
}

BitBoard MoveGenerator::computeEastPawnCaptures(const Position& pos, const BoardPerspective& persp, const BitBoard& relevant_pawns, const BitBoard& takeable_pieces) {
  // get up-west captures
  BitBoard east_cap = relevant_pawns.shift(persp.up_east);
  east_cap &= takeable_pieces;
  return east_cap;
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
  BitBoard west_cap = computeWestPawnCaptures(pos, persp, pawns, takeable_pieces);
  int west_offset = persp.side_to_move == Colour::White ? WHITE_PAWN_WEST_CAPTURE_OFFSET : BLACK_PAWN_WEST_CAPTURE_OFFSET;
  extractPawnMoves(west_cap, west_offset, MoveType::Capture, moves);

  BitBoard east_cap = computeEastPawnCaptures(pos, persp, pawns, takeable_pieces);
  int east_offset = persp.side_to_move == Colour::White ? WHITE_PAWN_EAST_CAPTURE_OFFSET : BLACK_PAWN_EAST_CAPTURE_OFFSET;
  extractPawnMoves(east_cap, east_offset, MoveType::Capture, moves);
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

  int west_offset = persp.side_to_move == Colour::White ? WHITE_PAWN_WEST_CAPTURE_OFFSET : BLACK_PAWN_WEST_CAPTURE_OFFSET;
  int east_offset = persp.side_to_move == Colour::White ? WHITE_PAWN_EAST_CAPTURE_OFFSET : BLACK_PAWN_EAST_CAPTURE_OFFSET;

  for (int piece = PieceType::Knight; piece < PieceType::King; piece++) {
    extractPawnMoves(west_cap, west_offset, MoveType::Capture, moves, static_cast<PieceType>(piece));
    extractPawnMoves(east_cap, east_offset, MoveType::Capture, moves, static_cast<PieceType>(piece));
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

  int west_offset = persp.side_to_move == Colour::White ? WHITE_PAWN_WEST_CAPTURE_OFFSET : BLACK_PAWN_WEST_CAPTURE_OFFSET;
  int east_offset = persp.side_to_move == Colour::White ? WHITE_PAWN_EAST_CAPTURE_OFFSET : BLACK_PAWN_EAST_CAPTURE_OFFSET;

  // compute possible en passant captures
  BitBoard west_cap = pawns.shift(persp.up_west);
  west_cap &= enpassant_targets;
  extractPawnMoves(west_cap, west_offset, MoveType::EnPassantCapture, moves);

  BitBoard east_cap = pawns.shift(persp.up_east);
  east_cap &= enpassant_targets;
  extractPawnMoves(east_cap, east_offset, MoveType::EnPassantCapture, moves);
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
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  while (!bishops.isEmpty()) {
    int bishop_index = bishops.popHighestSetBit();

    BitBoard bishop_moves = computeBishopMoves(pos, persp, bishop_index);
    
    extractPieceMoves(bishop_moves & ~enemy_pieces, bishop_index, MoveType::Quiet, moves);
    extractPieceMoves(bishop_moves & enemy_pieces, bishop_index, MoveType::Capture, moves);
  }
}

// NOTE: consider whether this method is excessive indirection
BitBoard MoveGenerator::computeBishopMoves(const Position& pos, const BoardPerspective& persp, int bishop_index) {
  int bishop_rank = indexToRank(bishop_index);
  int bishop_file = indexToFile(bishop_index);
  BitBoard bishop_moves;

  // north-east
  BitBoard ne_ray = rays[bishop_rank][bishop_file][Direction::NorthEast];
  bishop_moves |= computeBishopRayMoves(pos, persp, ne_ray, Direction::NorthEast, bishop_index);

  // south-east
  BitBoard se_ray = rays[bishop_rank][bishop_file][Direction::SouthEast];
  bishop_moves |= computeBishopRayMoves(pos, persp, se_ray, Direction::SouthEast, bishop_index);

  // south-west
  BitBoard sw_ray = rays[bishop_rank][bishop_file][Direction::SouthWest];
  bishop_moves |= computeBishopRayMoves(pos, persp, sw_ray, Direction::SouthWest, bishop_index);

  // north-west
  BitBoard nw_ray = rays[bishop_rank][bishop_file][Direction::NorthWest];
  bishop_moves |= computeBishopRayMoves(pos, persp, nw_ray, Direction::NorthWest, bishop_index);

  return bishop_moves;
}

BitBoard MoveGenerator::computeBishopRayMoves(const Position& pos, const BoardPerspective& persp, BitBoard ray, const Direction dir, int bishop_index) {
  int blocking_index;
  bool is_capture = false;
  BitBoard all_pieces = pos.getAllPiecesBitBoard();
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);

  BitBoard intersect = all_pieces & ray;
  // if intersection point is with an enemy piece then we can capture
  if (dir == Direction::NorthEast) {
    blocking_index = intersect.getLowestSetBit();
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    // if intersection is a capture the ray extends an extra square with a capture
    if (is_capture) blocking_index += 1;
    if (blocking_index >= 0) ray.clearBitsAbove(blocking_index);
  } else if (dir == Direction::SouthEast) {
    blocking_index = intersect.getHighestSetBit();
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    if (is_capture) blocking_index -= 1;
    if (blocking_index >= 0) ray.clearBitsBelow(blocking_index);
  } else if (dir == Direction::SouthWest) {
    blocking_index = intersect.getHighestSetBit();
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    if (is_capture) blocking_index -= 1;
    if (blocking_index >= 0) ray.clearBitsBelow(blocking_index);
  } else if (dir == Direction::NorthWest) {
    blocking_index = intersect.getLowestSetBit();
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    if (is_capture) blocking_index += 1;
    if (blocking_index >= 0) ray.clearBitsAbove(blocking_index);
  }

  return ray;
}

void MoveGenerator::generateRookMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard rooks = pos.getPieceBitBoard(persp.side_to_move, PieceType::Rook);
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  while (!rooks.isEmpty()) {
    int rook_index = rooks.popHighestSetBit();

    BitBoard rook_moves = computeRookMoves(pos, persp, rook_index);

    extractPieceMoves(rook_moves & ~enemy_pieces, rook_index, MoveType::Quiet, moves);
    extractPieceMoves(rook_moves & enemy_pieces, rook_index, MoveType::Capture, moves);
  }
}

BitBoard MoveGenerator::computeRookMoves(const Position& pos, const BoardPerspective& persp, int rook_index) {
  int rook_rank = indexToRank(rook_index);
  int rook_file = indexToFile(rook_index);
  BitBoard rook_moves;

  // north
  BitBoard n_ray = rays[rook_rank][rook_file][Direction::North];
  rook_moves |= computeRookRayMoves(pos, persp, n_ray, Direction::North, rook_index);

  // east
  BitBoard e_ray = rays[rook_rank][rook_file][Direction::East];
  rook_moves |= computeRookRayMoves(pos, persp, e_ray, Direction::East, rook_index);

  // south 
  BitBoard s_ray = rays[rook_rank][rook_file][Direction::South];
  rook_moves |= computeRookRayMoves(pos, persp, s_ray, Direction::South, rook_index);

  // west 
  BitBoard w_ray = rays[rook_rank][rook_file][Direction::West];
  rook_moves |= computeRookRayMoves(pos, persp, w_ray, Direction::West, rook_index);

  return rook_moves;
}

BitBoard MoveGenerator::computeRookRayMoves(const Position& pos, const BoardPerspective& persp, BitBoard ray, Direction dir, int rook_index) {
  int blocking_index;
  bool is_capture = false;
  BitBoard all_pieces = pos.getAllPiecesBitBoard();
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);

  BitBoard intersect = all_pieces & ray;
  // if intersection point is with an enemy piece then we can capture
  if (dir == Direction::North) {
    blocking_index = intersect.getLowestSetBit();
    // if the blocking piece is an enemy piece then we can capture 
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    // if intersection is a capture the ray extends an extra square with a capture
    if (is_capture) blocking_index += 1;
    if (blocking_index >= 0) ray.clearBitsAbove(blocking_index);
  } else if (dir == Direction::East) {
    blocking_index = intersect.getLowestSetBit();
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    if (is_capture) blocking_index += 1;
    if (blocking_index >= 0) ray.clearBitsAbove(blocking_index);
  } else if (dir == Direction::South) {
    blocking_index = intersect.getHighestSetBit();
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    if (is_capture) blocking_index -= 1;
    if (blocking_index >= 0) ray.clearBitsBelow(blocking_index);
  } else if (dir == Direction::West) {
    blocking_index = intersect.getHighestSetBit();
    if (blocking_index >= 0) is_capture = enemy_pieces.getBit(blocking_index);
    if (is_capture) blocking_index -= 1;
    if (blocking_index >= 0) ray.clearBitsBelow(blocking_index);
  }

  return ray;
}

void MoveGenerator::generateQueenMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  BitBoard queens = pos.getPieceBitBoard(persp.side_to_move, PieceType::Queen);
  BitBoard enemy_pieces = pos.getPieceBitBoard(persp.opponent, PieceType::All);
  while (!queens.isEmpty()) {
    int queen_index = queens.popHighestSetBit();
    BitBoard queen_moves = computeQueenMoves(pos, persp, queen_index);
    extractPieceMoves(queen_moves & ~enemy_pieces, queen_index, MoveType::Quiet, moves);
    extractPieceMoves(queen_moves & enemy_pieces, queen_index, MoveType::Capture, moves);
  }
}

BitBoard MoveGenerator::computeQueenMoves(const Position& pos, const BoardPerspective& persp, int queen_index) {
  BitBoard queen_moves;

  // queen is a combo of a bishop and rook so we can use their move generation functions
  queen_moves = computeBishopMoves(pos, persp, queen_index);
  queen_moves |= computeRookMoves(pos, persp, queen_index);

  return queen_moves;
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

// TODO: test this!
void MoveGenerator::generateCastles(const Position& pos, const BoardPerspective& persp, MoveVec& moves) {
  // NOTE: these masks should possibly go inside BoardPerspective ?
  // masks for the squares that must be empty 
  BitBoard kingside_mask;
  BitBoard queenside_mask;
  if (persp.side_to_move == Colour::White) {
    kingside_mask = BitBoard(96);
    queenside_mask = BitBoard(14);
  } else {
    kingside_mask = BitBoard(6917529027641081856);
    queenside_mask = BitBoard(1008806316530991104);
  }

  BitBoard all_pieces = pos.getAllPiecesBitBoard();

  if (pos.canCastle(persp.side_to_move, CastlingType::Kingside) &&
      (kingside_mask & all_pieces).isEmpty()) {
    // TODO: turn magic nums into constants
    int source = (persp.side_to_move == Colour::White) ? 4 : 60;        
    int dest = (persp.side_to_move == Colour::White) ? 6 : 62;        
    moves.emplace_back(source, dest, MoveType::KingsideCastle);
  }

  if (pos.canCastle(persp.side_to_move, CastlingType::Queenside) &&
      (queenside_mask & all_pieces).isEmpty()) {
    int source = (persp.side_to_move == Colour::White) ? 4 : 60;        
    int dest = (persp.side_to_move == Colour::White) ? 2 : 58;        
    moves.emplace_back(source, dest, MoveType::QueensideCastle);
  }
}

// TODO: test the absolute hell out of this
bool MoveGenerator::isLegal(const Move& move, const Position& pos, BoardPerspective& persp, int king_index, const BitBoard& checkers, const BitBoard& pinned_pieces) {
  if (move.source == king_index) {
    return isLegalKingMove(move, pos, persp, checkers);
  } else if (move.move_type == MoveType::EnPassantCapture) {
    return isLegalEnpassant(move, pos, persp, king_index, checkers, pinned_pieces);
  }
  return isLegalNonKingMove(move, pos, persp, king_index, checkers, pinned_pieces);
}

bool MoveGenerator::isLegalKingMove(const Move& move, const Position& pos, const BoardPerspective& persp, const BitBoard& checkers) {
  if (move.move_type == MoveType::KingsideCastle || move.move_type == MoveType::QueensideCastle) {
    return isLegalCastles(move, pos, persp, checkers);
  }

  // we need to move the king before calling getAttackers() to make sure he
  // isn't appearing to block an attack on the dest square
  Position moved_king_pos(pos);
  moved_king_pos.removePiece(persp.side_to_move, PieceType::King, move.source);
  moved_king_pos.addPiece(persp.side_to_move, PieceType::King, move.dest);

  return getAttackers(moved_king_pos, persp, move.dest).isEmpty();
}

bool MoveGenerator::isLegalNonKingMove(const Move& move, const Position& pos, const BoardPerspective& persp, int king_index, const BitBoard& checkers, const BitBoard& pinned_pieces) {
  int n_checkers = checkers.countSetBits();
  // if the king is in double check you must move the king
  if (n_checkers > 1) {
    return false;
  } else if (n_checkers == 1) {
    // if there is one piece checking the king we must take that piece or block
    // it (as long as the piece we use to do so is not itself pinned)
    int checker_index = checkers.getHighestSetBit();
    BitBoard checking_ray = getRayBetween(checker_index, king_index);
    if (!(checkers.getBit(move.dest) || checking_ray.getBit(move.dest)) ||
        pinned_pieces.getBit(move.source)) {
      return false;
    }
  } else {
    // if the king is not in check then we can make any move as long as the
    // piece isn't pinned in a different direction to the move
    if (pinned_pieces.getBit(move.source)) {
      return isLegalPinnedMove(move, pos, persp, king_index);
    }
  }

  return true;
}

bool MoveGenerator::isLegalCastles(const Move& move, const Position& pos, const BoardPerspective& persp, const BitBoard& checkers) {
  if (checkers.countSetBits() > 0) {
    return false;
  }

  // ensure king isn't moving through check when castling
  BoardBits relevant_square_mask = CASTLE_MASKS[persp.side_to_move].find(move.move_type)->second;
  BitBoard relevant_squares(relevant_square_mask);

  while (!relevant_squares.isEmpty()) {
    int square_index = relevant_squares.popHighestSetBit();
    if (!getAttackers(pos, persp, square_index).isEmpty()) {
      return false;
    }
  }
  return true;
}

// en passant requires special handling because it's only move where capturing
// piece does not end up on same square as captured piece. specifically,
// horizontal pins would be missed with normal move checking
bool MoveGenerator::isLegalEnpassant(const Move& move, const Position& pos, const BoardPerspective& persp, int king_index, const BitBoard& checkers, const BitBoard& pinned_pieces) {
  // NOTE: I am doing fewer checks than "purple-bot" here. he runs full non king
  // legal move checks on the altered pos - double check I am correct that only
  // concern is horizontal pin
  Position pos_copy = pos;
  pos_copy.removePiece(persp.opponent, PieceType::Pawn, move.dest + (persp.offset_sign * -8));
  BitBoard pinned_en_passant = getPinnedPieces(pos_copy, persp);
  if (pinned_en_passant.getBit(move.source)) {
    return false;
  }
  return isLegalNonKingMove(move, pos, persp, king_index, checkers, pinned_pieces);
}

bool MoveGenerator::isLegalPinnedMove(const Move& move, const Position& pos, const BoardPerspective& persp, int king_index) {
  // moving an absolutely pinned piece is only allowed if it's moving on the pinning ray
  BitBoard king_source_ray = getRayBetween(king_index, move.source);
  BitBoard king_dest_ray = getRayBetween(king_index, move.dest);
  // if piece is moving away from king then source must be on king -> dest ray,
  // if piece is moving towards king then dest must be on king -> source ray
  return (king_source_ray.getBit(move.dest) || king_dest_ray.getBit(move.source));
}

BitBoard MoveGenerator::getPinnedPieces(const Position& pos, const BoardPerspective& persp) {
  // using the same principle of symmetry as getAttackers() we can find pinned
  // pieces if rays from the king's position intersect with rays from enemy
  // pieces on a square containing a piece of ours
  int king_index = pos.getPieceBitBoard(persp.side_to_move, PieceType::King).getHighestSetBit();
  BitBoard our_pieces = pos.getPieceBitBoard(persp.side_to_move, PieceType::All);
  BoardPerspective opponent_persp(persp.opponent);
  BitBoard pinned_pieces;

  // bishop rays radiating outward from the king
  BitBoard outward_bishop_moves = computeBishopMoves(pos, opponent_persp, king_index);
  // bishop rays radiating from enemy bishops
  BitBoard inward_bishop_moves; 
  BitBoard enemy_bishops = pos.getPieceBitBoard(persp.opponent, PieceType::Bishop);
  // queen is just a combo rook/bishop so we can integrate checking queen pins into bishop and rook checks
  enemy_bishops |= pos.getPieceBitBoard(persp.opponent, PieceType::Queen);
  while (!enemy_bishops.isEmpty()) {
    int bishop_index = enemy_bishops.popHighestSetBit();
    // we're only interested in the bishop moves on the ray towards the king
    inward_bishop_moves |= computeBishopMoves(pos, opponent_persp, bishop_index) & getRayBetween(bishop_index, king_index);
  }
  pinned_pieces |= outward_bishop_moves & inward_bishop_moves & our_pieces;

  // rook rays radiating outward from the king
  BitBoard outward_rook_moves = computeRookMoves(pos, opponent_persp, king_index);
  // rook rays radiating from enemy rooks
  BitBoard inward_rook_moves; 
  BitBoard enemy_rooks = pos.getPieceBitBoard(persp.opponent, PieceType::Rook);
  // queen is just a combo rook/bishop so we can integrate checking queen pins into bishop and rook checks
  enemy_rooks |= pos.getPieceBitBoard(persp.opponent, PieceType::Queen);
  while (!enemy_rooks.isEmpty()) {
    int rook_index = enemy_rooks.popHighestSetBit();
    // we're only interested in the rook moves on the ray torwards the king
    inward_rook_moves |= computeRookMoves(pos, opponent_persp, rook_index)  & getRayBetween(rook_index, king_index);
  }
  pinned_pieces |= outward_rook_moves & inward_rook_moves & our_pieces;

  return pinned_pieces;
}

BitBoard MoveGenerator::getAttackers(const Position& pos, const BoardPerspective& persp, int square_bit_index) {
  // most chess moves are symmetrical - we can simply calculate attacks from all
  // piece types starting at the given square. If one of those attacks overlaps
  // with a enemy piece of the same type then the square is attacked

  int square_rank = indexToRank(square_bit_index);
  int square_file = indexToFile(square_bit_index);

  // could a bishop on the given square "attack" any enemy bishops?
  BitBoard bishop_moves = computeBishopMoves(pos, persp, square_bit_index); 
  BitBoard enemy_bishops = pos.getPieceBitBoard(persp.opponent, PieceType::Bishop);
  BitBoard attacking_bishops = enemy_bishops & bishop_moves;

  // could a rook on the given square "attack" any enemy rooks?
  BitBoard rook_moves = computeRookMoves(pos, persp, square_bit_index); 
  BitBoard enemy_rooks = pos.getPieceBitBoard(persp.opponent, PieceType::Rook);
  BitBoard attacking_rooks =enemy_rooks & rook_moves ;

  // queen is just a combo rook-bishop  
  BitBoard enemy_queens = pos.getPieceBitBoard(persp.opponent, PieceType::Queen);
  BitBoard attacking_queens = (bishop_moves & enemy_queens) | (rook_moves & enemy_queens); 

  // could a knight on the given square "attack" any enemy knights?
  BitBoard enemy_knights = pos.getPieceBitBoard(persp.opponent, PieceType::Knight);
  BitBoard attacking_knights = knight_moves[square_rank][square_file] & enemy_knights;

  // could a king on the given square "attack" any enemy kings?
  BitBoard enemy_king = pos.getPieceBitBoard(persp.opponent, PieceType::King);
  BitBoard attacking_kings = king_moves[square_rank][square_file] & enemy_king;

  // could a pawn on the given square "attack" any enemy pawns?
  BitBoard enemy_pawns = pos.getPieceBitBoard(persp.opponent, PieceType::Pawn);
  BitBoard target_square = BitBoard();
  target_square.setBit(square_bit_index);
  BitBoard attacking_pawns = computeWestPawnCaptures(pos, persp,target_square, enemy_pawns);
  attacking_pawns |= computeEastPawnCaptures(pos, persp,target_square, enemy_pawns);

  return attacking_bishops | attacking_rooks | attacking_queens | attacking_knights | attacking_kings | attacking_pawns;
}

MoveVec MoveGenerator::generateMoves(const Position& pos) {
  // directions switch depending on side to move
  BoardPerspective persp(pos.getSideToMove());
  int king_index = pos.getPieceBitBoard(persp.side_to_move, PieceType::King).getHighestSetBit();
  BitBoard checkers = getAttackers(pos, persp, king_index);
  BitBoard pinned_pieces = getPinnedPieces(pos, persp);

  MoveVec moves;

  // generates all the pseudo legal moves
  generatePawnMoves(pos, persp, moves);
  generateKnightMoves(pos, persp, moves);
  generateBishopMoves(pos, persp, moves);
  generateRookMoves(pos, persp, moves);
  generateQueenMoves(pos, persp, moves);
  generateKingMoves(pos, persp, moves);
  generateCastles(pos, persp, moves);

  // lambda that returns true if a move is illegal
  auto is_not_legal = [this, &pos , &persp, &checkers, &pinned_pieces, king_index](Move& move) {
    return !this->isLegal(move, pos, persp, king_index, checkers, pinned_pieces);
  };
  // prunes illegal moves from pseudo-legal moves
  auto legal_moves_end = std::remove_if(moves.begin(), moves.end(), is_not_legal);
  moves.erase(legal_moves_end, moves.end());

  // NOTE: do I really want to be returning this potentially large vector by value?
  return moves;
}

int MoveGenerator::computePerft(const Position& pos, int depth) {
  MoveVec moves = generateMoves(pos);
  if (depth == 1) {
    return moves.size();
  }
  int sum = 0;
  int move_total;
  for (const Move& move : moves) {
    Position resultant_pos = pos.applyMove(move);
    // sum += computePerft(resultant_pos, depth - 1);
    move_total = computePerft(resultant_pos, depth - 1);
    sum += move_total;
  }
  return sum;
}

void MoveGenerator::dividePerft(const Position& pos, int depth) {
  MoveVec moves = generateMoves(pos);
  if (depth == 1) {
    for (const Move& move : moves) {
      move.print(pos, true);
    }
    printf("total: %zu\n", moves.size());
    return;
  }
  int sum = 0;
  int move_total = 0;
  for (const Move& move : moves) {
    Position resultant_pos = pos.applyMove(move);
    move_total = computePerft(resultant_pos, depth - 1);
    sum += move_total;
    printf("%s : %d\n", move.to_string(pos, true).c_str(), move_total);
  }
  printf("total: %d\n", sum);
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

  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      for (int dest_rank = 0; dest_rank < 8; dest_rank++) {
        for (int dest_file = 0; dest_file < 8; dest_file++) {
          rays_between[rank][file][dest_rank][dest_file] = precomputeRaysBetween(rankFileToIndex(rank, file), rankFileToIndex(dest_rank, dest_file), rays);
        }
      }
    }
  }
}
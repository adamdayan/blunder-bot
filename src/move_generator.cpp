#include "move_generator.h"
#include "constants.h"
#include "utils.h"



// generates all rays from a given square
BitBoard precomputeRay(int start_rank, int start_file, Direction dir) {
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

void MoveGenerator::extractPawnMoves(BitBoard bb, int offset, MoveType type, MoveVec& moves, PieceType promotion) {
  while (!bb.isEmpty()) {
    int dest = bb.popHighestSetBit();
    // NOTE: offset sign changes per side
    moves.emplace_back(dest - offset, dest, type, promotion);
  }
}

void MoveGenerator::extractPieceMoves(BitBoard bb, int source, MoveType type, MoveVec& moves, PieceType promotion) {
  while (!bb.isEmpty()) {
    int dest = bb.popHighestSetBit();
    moves.emplace_back(source, dest, type, promotion);
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

MoveVec MoveGenerator::generateMoves(const Position& pos) {
  // directions switch depending on side to move
  BoardPerspective persp(pos.getSideToMove());

}

MoveGenerator::MoveGenerator() {
  // precomputes all rays and knight moves
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      for (int dir = Direction::North; dir <= Direction::NorthWest; dir++) {
        rays[rank][file][dir] = precomputeRay(rank, file, static_cast<Direction>(dir));
      }
      knight_moves[rank][file] = precomputeKnightMoves(rank, file);
    }
  }
}
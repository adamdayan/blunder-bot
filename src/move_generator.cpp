#include "move_generator.h"
#include "constants.h"

// generates all rays from a given square
BitBoard generateRay(int start_rank, int start_file, RayDirection dir) {
  BitBoard bb;
  // NOTE: would it be better to split these into separate functions?
  // Also, is there some better bit-shifty way to do this?
  switch (dir) {
    case RayDirection::North: {
      for (int rank = start_rank; rank < 8; rank++) {
        bb.setBit(rank, start_file);
      }
      break;
    }
    case RayDirection::NorthEast: {
      for (int rank = start_rank; rank < 8; rank++) {
        for (int file = start_file; file < 8; file++) {
          bb.setBit(rank, file);
        }
      }
      break;
    }
    case RayDirection::East : {
      for (int file = start_file; file < 8; file++) {
        bb.setBit(start_rank, file);
      }
      break;
    }
    case RayDirection::SouthEast : {
      for (int rank = start_rank; rank >= 0; rank--) {
        for (int file = start_file; file < 8; file++) {
          bb.setBit(rank, file);
        }
      }
      break;
    } 
    case RayDirection::South : {
      for (int rank = start_rank; rank >= 0; rank--) {
        bb.setBit(rank, start_file);
      }
      break;
    }
    case RayDirection::SouthWest : {
      for (int rank = start_rank; rank >= 0; rank--) {
        for (int file = start_file; file >= 0; file--) {
          bb.setBit(rank, file);
        }
        break;
      }
    }
    case RayDirection::West : {
      for (int file = start_file; file >= 0; file--) {
        bb.setBit(start_rank, file);
      }
      break;
    }
    case RayDirection::NorthWest : {
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

MoveGenerator::MoveGenerator() {
  // precomputes all rays and knight moves
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      for (int dir = RayDirection::North; dir <= RayDirection::NorthWest; dir++) {
        rays[rank][file][dir] = generateRay(rank, file, static_cast<RayDirection>(dir));
      }
      knight_moves[rank][file] = generateKnightMoves(rank, file);
    }
  }
}
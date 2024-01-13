#include "move_generator.h"
#include "constants.h"

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

BitBoard generateKnightMoves(int rank, int file) {
  BitBoard bb;

  return bb;
}

MoveGenerator::MoveGenerator() {
  // precomputes all rays
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      for (int dir = RayDirection::North; dir <= RayDirection::NorthWest; dir++) {
        rays[rank][file][dir] = generateRay(rank, file, static_cast<RayDirection>(dir));
      }      
    }
  }
}
#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <array>
#include <vector>

#include "position.h"
#include "utils.h"

class Move {

};

class MoveGenerator {
  public:
    // do all precomputation here
    MoveGenerator();
    std::vector<Move> generateMoves(const Position& position);

    BoardMatrix<std::array<BitBoard, 8>> rays;
    BoardMatrix<BitBoard> knight_moves;
};

#endif // MOVE_GENERATOR_H
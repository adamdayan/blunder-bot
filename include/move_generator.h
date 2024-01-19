#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <array>
#include <vector>

#include "position.h"
#include "utils.h"
#include "constants.h"

// TODO: decide what on earth this class will look like
class Move {
  Move(int source, int dest);
};

class BoardPerspective {
  public:
    BoardPerspective(Colour side_to_move) : side_to_move(side_to_move) {
      if (side_to_move == Colour::White) {
        opponent = Colour::Black;
        pawn_start_rank = RANK2;  
        pawn_promote_rank = RANK8;
        pawn_pre_promote_rank = RANK7;
        double_push_possible = RANK3;
        up = Direction::North;
        up_east = Direction::NorthEast;
        down_east = Direction::SouthEast;
        down = Direction::South;
        down_west = Direction::SouthWest;
        up_west = Direction::NorthWest;
      } else {
        opponent = Colour::White;
        pawn_start_rank = RANK7;
        pawn_promote_rank = RANK1;
        pawn_pre_promote_rank = RANK2;
        double_push_possible = RANK6;
        up = Direction::South; 
        up = Direction::South;
        up_east = Direction::SouthEast;
        down_east = Direction::NorthEast;
        down = Direction::North;
        down_west = Direction::NorthWest;
        up_west = Direction::SouthWest;
      }
    }
    Colour side_to_move;
    Colour opponent;
    BoardBits pawn_start_rank;
    BoardBits pawn_promote_rank;
    BoardBits pawn_pre_promote_rank;
    BoardBits double_push_possible;
    Direction up;
    Direction up_east;
    Direction down_east; 
    Direction down;
    Direction down_west;
    Direction up_west;
};

class MoveGenerator {
  public:
    // do all precomputation here
    MoveGenerator();
    // computes all legal moves
    std::vector<Move> generateMoves(const Position& position);
    BitBoard computePawnMoves(const Position& pos, const BoardPerspective& persp);
    BitBoard computeQuietPawnPushes(const Position& pos, const BoardPerspective& persp);
    BitBoard computeSinglePawnPushes(const Position& pos, const BoardPerspective& persp);
    BitBoard computeDoublePawnPushes(const Position& pos, const BoardPerspective& persp, BitBoard single_pushes);
    BitBoard computePawnCaptures(const Position& pos, const BoardPerspective& persp);
    BitBoard computeEnPassant(const Position& pos);
    BitBoard computePromotions(const Position& pos);

  private:
    BoardMatrix<std::array<BitBoard, 8>> rays;
    BoardMatrix<BitBoard> knight_moves;
};

#endif // MOVE_GENERATOR_H
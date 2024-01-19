#include <catch2/catch_test_macros.hpp>

#include "move_generator.h"
#include "position.h"

TEST_CASE("test single pawn pushes", "[move_generator]") {
  MoveGenerator move_gen;
  Position pos(start_position);
  BoardPerspective persp(pos.getSideToMove());
  BitBoard single_pushes = move_gen.computeSinglePawnPushes(pos, persp);
  REQUIRE(single_pushes.board == RANK3);
}

TEST_CASE("test double pawn pushes", "[move_generator]") {
  MoveGenerator move_gen;
  Position pos(start_position);
  BoardPerspective persp(pos.getSideToMove());
  BitBoard single_pushes = move_gen.computeSinglePawnPushes(pos, persp);
  BitBoard double_pushes = move_gen.computeDoublePawnPushes(pos, persp, single_pushes); 
  REQUIRE(double_pushes.board == RANK4);
}
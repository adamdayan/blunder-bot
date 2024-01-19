#include <catch2/catch_test_macros.hpp>
#include <string>

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

TEST_CASE("test pawn promotion", "[move_generator]") {
  MoveGenerator move_gen;
  std::string pawn_about_to_promote_position =  "8/1P6/8/8/8/8/8/8 w KQkq - 0 1";
  Position pos(pawn_about_to_promote_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generatePromotions(pos, persp, moves);

  for (int piece = PieceType::Knight; piece < PieceType::King; piece++) {
    REQUIRE(moves[piece-1].source == 49);
    REQUIRE(moves[piece-1].dest == 57);
    REQUIRE(moves[piece-1].move_type == MoveType::Quiet);
    REQUIRE(moves[piece-1].promotion == piece);
  }
}
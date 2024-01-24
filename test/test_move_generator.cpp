#include <catch2/catch_test_macros.hpp>
#include <string>

#include "move_generator.h"
#include "position.h"
#include "utils.h"

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

// TODO: test more complex promotion scenarios
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

// TODO: test more complex en passant scenarios
TEST_CASE("test en passant", "[move_generator]") {
  MoveGenerator move_gen;
  std::string en_passant_position =  "8/8/8/8/4pP2/8/8/8 b - f3 0 1";
  Position pos(en_passant_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateEnPassant(pos, persp, moves);

  REQUIRE(moves.size() == 1);
  REQUIRE(moves[0].source == rankFileToIndex(3, 4));
  REQUIRE(moves[0].dest == rankFileToIndex(2, 5));
  REQUIRE(moves[0].move_type == MoveType::EnPassantCapture);
}

TEST_CASE("test knight moves", "[move_generator]") {
  MoveGenerator move_gen;
  std::string knight_position = "8/8/8/8/4r3/8/3N4/8 w - - 0 0";
  Position pos(knight_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateKnightMoves(pos, persp, moves);
  REQUIRE(moves.size() == 6);
  REQUIRE(moves[5].source == rankFileToIndex(1, 3));
  REQUIRE(moves[5].dest == rankFileToIndex(3, 4));
  REQUIRE(moves[5].move_type == MoveType::Capture);
  for (int i = 0; i < 5; i++) {
    REQUIRE(moves[i].move_type != MoveType::Capture);
  }
}
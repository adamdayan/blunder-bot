#include <catch2/catch_test_macros.hpp>
#include <string>

#include "move_generator.h"
#include "position.h"
#include "utils.h"

TEST_CASE("test computeSinglePawnPushes()", "[move_generator]") {
  MoveGenerator move_gen;
  Position pos(start_position);
  BoardPerspective persp(pos.getSideToMove());
  BitBoard single_pushes = move_gen.computeSinglePawnPushes(pos, persp);
  REQUIRE(single_pushes.board == RANK3);
}

TEST_CASE("test computeDoublePawnPushes()", "[move_generator]") {
  MoveGenerator move_gen;
  Position pos(start_position);
  BoardPerspective persp(pos.getSideToMove());
  BitBoard single_pushes = move_gen.computeSinglePawnPushes(pos, persp);
  BitBoard double_pushes = move_gen.computeDoublePawnPushes(pos, persp, single_pushes); 
  REQUIRE(double_pushes.board == RANK4);
}

// TODO: test more complex promotion scenarios
TEST_CASE("test generatePromotions()", "[move_generator]") {
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
TEST_CASE("test generateEnPassant()", "[move_generator]") {
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

TEST_CASE("test generateKnightMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string knight_position = "8/8/8/8/4r3/8/3N4/8 w - - 0 0";
  Position pos(knight_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateKnightMoves(pos, persp, moves);
  // final move should be taking the rook
  REQUIRE(moves.size() == 6);
  REQUIRE(moves[5].source == rankFileToIndex(1, 3));
  REQUIRE(moves[5].dest == rankFileToIndex(3, 4));
  REQUIRE(moves[5].move_type == MoveType::Capture);
  // check the other moves are quiet
  for (int i = 0; i < 5; i++) {
    REQUIRE(moves[i].move_type == MoveType::Quiet);
  }
}

TEST_CASE("test generateBishopMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string bishop_position = "8/8/8/2p1p3/3B4/2P1P3/8/8 w - - 0 0";
  Position pos(bishop_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateBishopMoves(pos, persp, moves);  

  REQUIRE(moves.size() == 2);
  // ne ray is first
  REQUIRE(moves[0].source == rankFileToIndex(3, 3));
  REQUIRE(moves[0].dest == rankFileToIndex(4, 4));
  REQUIRE(moves[0].move_type == MoveType::Capture);
  // sw ray
  REQUIRE(moves[1].source == rankFileToIndex(3, 3));
  REQUIRE(moves[1].dest == rankFileToIndex(4, 2));
  REQUIRE(moves[1].move_type == MoveType::Capture);
}

TEST_CASE("test generateRookMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string rook_position = "8/8/8/8/3p4/2PRp3/3P4/8 w - - 0 0";
  Position pos(rook_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateRookMoves(pos, persp, moves);  

  REQUIRE(moves.size() == 2);
  // n ray is first
  REQUIRE(moves[0].source == rankFileToIndex(2, 3));
  REQUIRE(moves[0].dest == rankFileToIndex(3, 3));
  REQUIRE(moves[0].move_type == MoveType::Capture);
  // e ray
  REQUIRE(moves[1].source == rankFileToIndex(2, 3));
  REQUIRE(moves[1].dest == rankFileToIndex(2, 4));
  REQUIRE(moves[1].move_type == MoveType::Capture);
}

TEST_CASE("test generateRookMoves() board edge", "[move_generator]") {
  MoveGenerator move_gen;
  std::string rook_position = "8/8/8/7P/6PR/7P/8/8 w - - 0 0";
  Position pos(rook_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateRookMoves(pos, persp, moves);  

  REQUIRE(moves.size() == 0);
}

TEST_CASE("test generateQueenMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string queen_position = "8/8/8/8/2PPp3/2pQP3/2pPP3/8 w - - 0 0";
  Position pos(queen_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateQueenMoves(pos, persp, moves);  

  REQUIRE(moves.size() == 3);
  
  // ne ray
  REQUIRE(moves[0].source == rankFileToIndex(2, 3));
  REQUIRE(moves[0].dest == rankFileToIndex(3, 4));
  REQUIRE(moves[0].move_type == MoveType::Capture);
  // w ray
  REQUIRE(moves[1].source == rankFileToIndex(2, 3));
  REQUIRE(moves[1].dest == rankFileToIndex(2, 2));
  REQUIRE(moves[1].move_type == MoveType::Capture);
  // sw ray
  REQUIRE(moves[2].source == rankFileToIndex(2, 3));
  REQUIRE(moves[2].dest == rankFileToIndex(1, 2));
  REQUIRE(moves[2].move_type == MoveType::Capture);
}

TEST_CASE("test generateKingMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string king_position = "8/8/8/8/2P1P3/2PKP3/2ppP3/8";
  Position pos(king_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateKingMoves(pos, persp, moves);  

  REQUIRE(moves.size() == 3);

  REQUIRE(moves[0].source == rankFileToIndex(2, 3));
  REQUIRE(moves[0].dest == rankFileToIndex(3, 3));
  REQUIRE(moves[0].move_type == MoveType::Quiet);

  REQUIRE(moves[1].source == rankFileToIndex(2, 3));
  REQUIRE(moves[1].dest == rankFileToIndex(1, 3));
  REQUIRE(moves[1].move_type == MoveType::Capture);

  REQUIRE(moves[2].source == rankFileToIndex(2, 3));
  REQUIRE(moves[2].dest == rankFileToIndex(1, 2));
  REQUIRE(moves[2].move_type == MoveType::Capture);
}

TEST_CASE("test valid kingside generateCastles()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string kingside_castle_position = "8/8/8/8/8/8/8/4K2R w K - 0 0";
  Position pos(kingside_castle_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateCastles(pos, persp, moves);
  // final move should be taking the rook
  REQUIRE(moves.size() == 1);
  REQUIRE(moves[0].source == rankFileToIndex(0, 4));
  REQUIRE(moves[0].dest == rankFileToIndex(0, 6));
  REQUIRE(moves[0].move_type == MoveType::KingsideCastle);
}

TEST_CASE("test valid queenside generateCastles()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string queenside_castle_position = "r3k3/8/8/8/8/8/8/8 b q - 0 0";
  Position pos(queenside_castle_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateCastles(pos, persp, moves);
  // final move should be taking the rook
  REQUIRE(moves.size() == 1);
  REQUIRE(moves[0].source == rankFileToIndex(7, 4));
  REQUIRE(moves[0].dest == rankFileToIndex(7, 2));
  REQUIRE(moves[0].move_type == MoveType::QueensideCastle);
}

TEST_CASE("test invalid kingside generateCastles()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string kingside_castle_position = "8/8/8/8/8/8/8/4K1NR w K - 0 0";
  Position pos(kingside_castle_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateCastles(pos, persp, moves);
  // final move should be taking the rook
  REQUIRE(moves.size() == 0);
}

TEST_CASE("test invalid queenside generateCastles()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string queenside_castle_position = "r2qk3/8/8/8/8/8/8/8 b q - 0 0";
  Position pos(queenside_castle_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateCastles(pos, persp, moves);
  // final move should be taking the rook
  REQUIRE(moves.size() == 0);
}
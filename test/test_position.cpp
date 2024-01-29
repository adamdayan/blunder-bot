#include <catch2/catch_test_macros.hpp>

#include "position.h"
#include "constants.h"
#include "utils.h"

TEST_CASE("test parseFEN on empty_position", "[position]") {
  Position pos(empty_board);
  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = PieceType::Pawn; piece <= PieceType::All; piece++) {
      BitBoard piece_board = pos.getPieceBitBoard(
          static_cast<Colour>(colour), static_cast<PieceType>(piece));
      REQUIRE(piece_board.isEmpty());
    }
  }

  REQUIRE(pos.getAllPiecesBitBoard().isEmpty());
  REQUIRE(pos.getEnpassantBitBoard().isEmpty());
}

TEST_CASE("test single piece makeMove", "[position]") {
  Position pos("8/8/8/8/8/8/8/Q7 w KQkq - 0 1");
  Move queen_move = Move(0, 7, MoveType::Quiet);
  pos.makeMove(queen_move);

  REQUIRE(pos.getPieceType(Colour::White, 0) == PieceType::None); 
  REQUIRE(pos.getPieceType(Colour::White, 7) == PieceType::Queen); 
  REQUIRE_FALSE(pos.getAllPiecesBitBoard().getBit(0));
  REQUIRE(pos.getAllPiecesBitBoard().getBit(7));
}

TEST_CASE("test queenside castle makeMove()", "[position]") {
  Position pos("8/8/8/8/8/8/8/R3K3 w KQkq - 0 1");
  Move castle_move = Move(4, 2, MoveType::QueensideCastle);
  pos.makeMove(castle_move);

  REQUIRE(pos.getPieceType(Colour::White, 4) == PieceType::None); 
  REQUIRE(pos.getPieceType(Colour::White, 2) == PieceType::King); 
  REQUIRE(pos.getPieceType(Colour::White, 3) == PieceType::Rook); 
  REQUIRE_FALSE(pos.getAllPiecesBitBoard().getBit(4));
  REQUIRE_FALSE(pos.getAllPiecesBitBoard().getBit(0));
  REQUIRE(pos.getAllPiecesBitBoard().getBit(2));
  REQUIRE(pos.getAllPiecesBitBoard().getBit(3));
  REQUIRE_FALSE(pos.canCastle(Colour::White, CastlingType::Kingside));
  REQUIRE_FALSE(pos.canCastle(Colour::White, CastlingType::Queenside));
}

TEST_CASE("test kingside castle makeMove()", "[position]") {
  Position pos("4k2r/8/8/8/8/8/8/8 b KQkq - 0 1");
  Move castle_move = Move(60, 62, MoveType::KingsideCastle);
  pos.makeMove(castle_move);

  REQUIRE(pos.getPieceType(Colour::Black, 59) == PieceType::None); 
  REQUIRE(pos.getPieceType(Colour::Black, 62) == PieceType::King); 
  REQUIRE(pos.getPieceType(Colour::Black, 61) == PieceType::Rook); 
  REQUIRE_FALSE(pos.getAllPiecesBitBoard().getBit(59));
  REQUIRE_FALSE(pos.getAllPiecesBitBoard().getBit(63));
  REQUIRE(pos.getAllPiecesBitBoard().getBit(62));
  REQUIRE(pos.getAllPiecesBitBoard().getBit(61));
  REQUIRE_FALSE(pos.canCastle(Colour::Black, CastlingType::Kingside));
  REQUIRE_FALSE(pos.canCastle(Colour::Black, CastlingType::Queenside));
}

TEST_CASE("test rook move remove castling rights makeMove()", "[position]") {
  Position pos("8/8/8/8/8/8/8/R3K3 w KQkq - 0 1");
  Move rook_move = Move(0, 1, MoveType::Quiet);
  pos.makeMove(rook_move);

  REQUIRE_FALSE(pos.canCastle(Colour::White, CastlingType::Queenside));
}

TEST_CASE("test king move remove castling rights makeMove()", "[position]") {
  Position pos("8/8/8/8/8/8/8/R3K3 w KQkq - 0 1");
  Move king_move = Move(4, 3, MoveType::Quiet);
  pos.makeMove(king_move);

  REQUIRE_FALSE(pos.canCastle(Colour::White, CastlingType::Kingside));
  REQUIRE_FALSE(pos.canCastle(Colour::White, CastlingType::Queenside));
}

TEST_CASE("test enpassant makeMove()", "[position]") {
  std::string en_passant_position =  "8/8/8/8/4pP2/8/8/8 b - f3 0 1";
  Position pos(en_passant_position);
  Move ep_move = Move(rankFileToIndex(3, 4), rankFileToIndex(2, 5), MoveType::EnPassantCapture);
  pos.makeMove(ep_move);

  REQUIRE(pos.getPieceType(Colour::Black, rankFileToIndex(2, 5)) == PieceType::Pawn);
  REQUIRE(pos.getPieceType(Colour::White, rankFileToIndex(3, 5)) == PieceType::None);
  REQUIRE(pos.getAllPiecesBitBoard().getBit(rankFileToIndex(2, 5)));
  REQUIRE_FALSE(pos.getAllPiecesBitBoard().getBit(rankFileToIndex(3, 4)));
  REQUIRE_FALSE(pos.getAllPiecesBitBoard().getBit(rankFileToIndex(3, 5)));
}


// TODO: add more Position tests
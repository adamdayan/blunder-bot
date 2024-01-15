#include <catch2/catch_test_macros.hpp>

#include "position.h"
#include "constants.h"

TEST_CASE("test parseFEN on empty_position", "[position]") {
  Position pos(empty_board);
  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = PieceType::Pawn; piece <= PieceType::All; piece++) {
      BitBoard piece_board = pos.getPieceBitBoard(
          static_cast<Colour>(colour), static_cast<PieceType>(piece));
      REQUIRE(piece_board.empty());
    }
  }

  REQUIRE(pos.getAllPiecesBitBoard().empty());
  REQUIRE(pos.getEnpassantBitBoard().empty());
}

// TODO: add more Position tests
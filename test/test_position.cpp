#include <catch2/catch_test_macros.hpp>

#include "position.h"
#include "constants.h"
#include "utils.h"
#include "squares.h"

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

TEST_CASE("test quiet promotion makeMove()", "[position]") {
  std::string quiet_promotion_position =  "8/7P/8/8/8/8/8/8 w - - 0 1";
  Position pos(quiet_promotion_position);
  Move prom_move = Move(rankFileToIndex(6, 7), rankFileToIndex(7, 7), MoveType::Quiet, PieceType::Rook);
  pos.makeMove(prom_move);

  REQUIRE(pos.getPieceType(Colour::White, rankFileToIndex(7, 7)) == PieceType::Rook);
  REQUIRE(pos.getPieceType(Colour::White, rankFileToIndex(6, 7)) == PieceType::None);
  REQUIRE(pos.getPieceBitBoard(Colour::White, PieceType::Pawn).isEmpty());
}

TEST_CASE("test capture promotion makeMove()", "[position]") {
  std::string cap_promotion_position =  "6q1/7P/8/8/8/8/8/8 w - - 0 1";
  Position pos(cap_promotion_position);
  Move prom_move = Move(rankFileToIndex(6, 7), rankFileToIndex(7, 6), MoveType::Capture, PieceType::Rook);
  pos.makeMove(prom_move);

  REQUIRE(pos.getPieceType(Colour::White, rankFileToIndex(7, 6)) == PieceType::Rook);
  REQUIRE(pos.getPieceType(Colour::White, rankFileToIndex(6, 7)) == PieceType::None);
  REQUIRE(pos.getPieceType(Colour::Black, rankFileToIndex(7, 6)) == PieceType::None);
  REQUIRE(pos.getPieceBitBoard(Colour::White, PieceType::Pawn).isEmpty());
  REQUIRE(pos.getPieceBitBoard(Colour::Black, PieceType::Queen).isEmpty());
}

TEST_CASE("test hash after move", "[position]") {
  ZobristHash::initialiseKeys();
  std::string pre_move_position =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 1";
  Position pos(pre_move_position);
  Move move = Move(9, 17, MoveType::Quiet);
  pos.makeMove(move);

  std::string post_move_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/1PN2Q1p/2PBBPPP/R3K2R b KQkq - 0 1";
  Position post_pos(post_move_position);

  REQUIRE(pos.getHash() != 0);
  REQUIRE(pos.getHash() == post_pos.getHash());
}

TEST_CASE("test hash after castling", "[position]") {
  ZobristHash::initialiseKeys();
  std::string pre_castle_position =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 1";
  Position pos(pre_castle_position);
  Move castle_move = Move(4, 6, MoveType::KingsideCastle);
  pos.makeMove(castle_move);

  std::string post_castle_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R4RK1 b kq - 0 1";
  Position post_pos(post_castle_position);

  REQUIRE(pos.getHash() != 0);
  REQUIRE(pos.getHash() == post_pos.getHash());
}

TEST_CASE("test hash after en passant", "[position]") {
  ZobristHash::initialiseKeys();
  std::string pre_ep_position =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 1";
  Position pos(pre_ep_position);
  Move ep_move = Move(14, rankFileToIndex(3, 6), MoveType::Quiet);
  pos.makeMove(ep_move);

  std::string post_ep_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P1P1/2N2Q1p/1PPBBP1P/R3K2R b KQkq g3 0 1";
  Position post_pos(post_ep_position);

  REQUIRE(pos.getHash() != 0);
  REQUIRE(pos.getHash() == post_pos.getHash());
}

TEST_CASE("test drawByRepetition()", "[position]") {
  ZobristHash::initialiseKeys();
  std::string starting_pos =  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R w KQkq - 0 1";
  Position pos(starting_pos);
  
  // repeat starting position further 2 times
  for (int i = 0; i < 2; i++) {
    Move w1_move = Move(rankFileToIndex(4, 4), rankFileToIndex(2, 3), MoveType::Quiet);
    pos.makeMove(w1_move);

    Move b1_move = Move(rankFileToIndex(5, 1), rankFileToIndex(7, 2), MoveType::Quiet);
    pos.makeMove(b1_move);

    Move w2_move = Move(rankFileToIndex(2, 3), rankFileToIndex(4, 4), MoveType::Quiet);
    pos.makeMove(w2_move);

    REQUIRE_FALSE(pos.isDrawByRepetition());

    Move b2_move = Move(rankFileToIndex(7, 2), rankFileToIndex(5, 1), MoveType::Quiet);
    pos.makeMove(b2_move);
  }

  REQUIRE(pos.isDrawByRepetition());
}

TEST_CASE("test flipped Position", "[position]") {
  Position pos("4k2r/6pp/8/8/8/8/PP6/2B1K3 w k - 0 1");
  Position flipped_pos = pos.flip();

  BitBoard all_pieces = flipped_pos.getAllPiecesBitBoard();
  REQUIRE((all_pieces.getBit(H1) && all_pieces.getBit(H2) &&
          all_pieces.getBit(G2) && all_pieces.getBit(C8) &&
          all_pieces.getBit(A7) && all_pieces.getBit(B7)));

  REQUIRE(flipped_pos.getPieceBitBoard(Colour::White, PieceType::Bishop)
              .getHighestSetBit() == C8);

  REQUIRE(flipped_pos.canCastle(Colour::White, CastlingType::Kingside));
  REQUIRE_FALSE(flipped_pos.canCastle(Colour::Black, CastlingType::Kingside));
}

// TODO: add more Position tests
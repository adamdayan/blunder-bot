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

TEST_CASE("test easy generateKnightMoves()", "[move_generator]") {
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

TEST_CASE("test corner generateKnightMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string knight_position = "8/8/8/8/8/8/8/6N1 w - - 0 0";
  Position pos(knight_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateKnightMoves(pos, persp, moves);

  REQUIRE(moves.size() == 3);
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

TEST_CASE("test top corner generateBishopMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string bishop_position = "5N1N/6b1/5p1p/8/8/8/8/8 b - - 0 0";
  Position pos(bishop_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateBishopMoves(pos, persp, moves);  

  REQUIRE(moves.size() == 2);
  // ne ray is first
  REQUIRE(moves[0].source == rankFileToIndex(6, 6));
  REQUIRE(moves[0].dest == rankFileToIndex(7, 7));
  REQUIRE(moves[0].move_type == MoveType::Capture);
  // nw ray
  REQUIRE(moves[1].source == rankFileToIndex(6, 6));
  REQUIRE(moves[1].dest == rankFileToIndex(7, 5));
  REQUIRE(moves[1].move_type == MoveType::Capture);
}

TEST_CASE("test bottom corner generateBishopMoves()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string bishop_position = "8/8/8/8/8/P1p5/1b6/N1p5 b - - 0 0";
  Position pos(bishop_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  move_gen.generateBishopMoves(pos, persp, moves);  

  REQUIRE(moves.size() == 2);

  REQUIRE(moves[0].source == rankFileToIndex(1, 1));
  REQUIRE(moves[0].dest == rankFileToIndex(2, 0));
  REQUIRE(moves[0].move_type == MoveType::Capture);

  REQUIRE(moves[1].source == rankFileToIndex(1, 1));
  REQUIRE(moves[1].dest == rankFileToIndex(0, 0));
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

TEST_CASE("test false isLegalKingMove()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string illegal_king_move_position = "1rr5/8/8/8/8/8/P7/K7 w - - 0 0";
  Position pos(illegal_king_move_position);
  BoardPerspective persp(pos.getSideToMove());
  int king_index = pos.getPieceBitBoard(persp.side_to_move, PieceType::King).getHighestSetBit();
  BitBoard checkers = move_gen.getAttackers(pos, persp, king_index);

  Move move0(0, 1, MoveType::Quiet);
  REQUIRE_FALSE(move_gen.isLegalKingMove(move0, pos, persp, checkers));

  Move move1(0, 9, MoveType::Quiet);
  REQUIRE_FALSE(move_gen.isLegalKingMove(move1, pos, persp, checkers));
}

TEST_CASE("test true isLegalKingMove()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string illegal_king_move_position = "1rr5/8/8/8/8/P7/8/K7 w - - 0 0";
  Position pos(illegal_king_move_position);
  BoardPerspective persp(pos.getSideToMove());
  int king_index = pos.getPieceBitBoard(persp.side_to_move, PieceType::King).getHighestSetBit();
  BitBoard checkers = move_gen.getAttackers(pos, persp, king_index);

  Move move(0, 8, MoveType::Quiet);
  REQUIRE(move_gen.isLegalKingMove(move, pos, persp, checkers));
}

TEST_CASE("test getAttackers()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string attacked_square_position = "1p1R4/8/N2B4/8/8/8/8/1R6 b - - 0 0";
  Position pos(attacked_square_position);
  BoardPerspective persp(pos.getSideToMove());
  BitBoard attackers = move_gen.getAttackers(pos, persp, 57);
  REQUIRE(attackers.countSetBits() == 4);
  REQUIRE(attackers.board == pos.getPieceBitBoard(persp.opponent, PieceType::All).board);
}

TEST_CASE("test getPinnedPieces()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string pinned_position = "8/8/8/8/3b4/8/1P6/KQ1r4 w - - 0 0";
  Position pos(pinned_position);
  BoardPerspective persp(pos.getSideToMove());
  BitBoard pinned_pieces = move_gen.getPinnedPieces(pos, persp);
  REQUIRE(pinned_pieces.countSetBits() == 2);
  REQUIRE(pinned_pieces.getBit(1));
  REQUIRE(pinned_pieces.getBit(9));
}

TEST_CASE("test pinned + check isLegal()", "[move_generator]") {
  MoveGenerator move_gen;
  std::string pinned_check_position = "8/8/8/Q1pk4/3p4/8/B7/8 b - - 0 0";
  Position pos(pinned_check_position);
  BoardPerspective persp(pos.getSideToMove());
  int king_index = pos.getPieceBitBoard(Colour::Black, PieceType::King).getHighestSetBit();
  BitBoard checkers = move_gen.getAttackers(pos, persp, king_index);
  BitBoard pinned_pieces = move_gen.getPinnedPieces(pos, persp);

  Move pinned_move(rankFileToIndex(4, 2), rankFileToIndex(3, 2), MoveType::Quiet);
  REQUIRE_FALSE(move_gen.isLegal(pinned_move, pos, persp, king_index, checkers, pinned_pieces));

  Move check_move(king_index, rankFileToIndex(3, 2), MoveType::Quiet);
  REQUIRE_FALSE(move_gen.isLegal(check_move, pos, persp, king_index, checkers, pinned_pieces));

  Move pawn_move(rankFileToIndex(3, 3), rankFileToIndex(2, 3), MoveType::Quiet);
  REQUIRE_FALSE(move_gen.isLegal(pawn_move, pos, persp, king_index, checkers, pinned_pieces));

  Move legal_move(king_index, rankFileToIndex(4, 4), MoveType::Quiet);
  REQUIRE(move_gen.isLegal(legal_move, pos, persp, king_index, checkers, pinned_pieces));

  legal_move = Move(king_index,rankFileToIndex(3, 4), MoveType::Quiet);
  REQUIRE(move_gen.isLegal(legal_move, pos, persp, king_index, checkers, pinned_pieces));

  legal_move = Move(king_index,rankFileToIndex(5, 4), MoveType::Quiet);
  REQUIRE_FALSE(move_gen.isLegal(legal_move, pos, persp, king_index, checkers, pinned_pieces));

  legal_move = Move(king_index,rankFileToIndex(5, 3), MoveType::Quiet);
  REQUIRE(move_gen.isLegal(legal_move, pos, persp, king_index, checkers, pinned_pieces));

  legal_move = Move(king_index,rankFileToIndex(5, 2), MoveType::Quiet);
  REQUIRE(move_gen.isLegal(legal_move, pos, persp, king_index, checkers, pinned_pieces));
}

TEST_CASE("test illegal queenside castling", "[move_generator]") {
  MoveGenerator move_gen;
  std::string castle_position = "8/8/8/6b1/8/8/8/R3K3 w Q - 0 0";
  Position pos(castle_position);
  BoardPerspective persp(pos.getSideToMove());
  MoveVec moves;
  int king_index = pos.getPieceBitBoard(persp.side_to_move, PieceType::King).getHighestSetBit();
  BitBoard checkers = move_gen.getAttackers(pos, persp, king_index);

  move_gen.generateCastles(pos, persp, moves);
  REQUIRE(moves.size() == 1);

  REQUIRE_FALSE(move_gen.isLegalCastles(moves[0], pos, persp, checkers));
}

TEST_CASE("test generateMoves() initial position move count", "[move_generator]") {
  MoveGenerator move_gen;
  std::string init_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Position pos(init_position);

  MoveVec moves = move_gen.generateMoves(pos);
  REQUIRE(moves.size() == 20);
}

// TODO: maybe add a dedicated series of perft tests
TEST_CASE("test perft(4) initial position", "[move_generator]") {
  MoveGenerator move_gen;
  std::string init_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Position pos(init_position);
  int move_count = move_gen.computePerft(pos, 4);
  REQUIRE(move_count == 197281);
}

TEST_CASE("test generateMoves() kiwipete position move count", "[move_generator]") {
  MoveGenerator move_gen;
  std::string kp_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
  Position pos(kp_position);

  MoveVec moves = move_gen.generateMoves(pos);
  REQUIRE(moves.size() == 48);

  int capture_cnt = 0;
  int castle_cnt = 0;
  for (const auto& move : moves) {
    if (move.move_type == MoveType::Capture || move.move_type == MoveType::EnPassantCapture) {
      capture_cnt++;
    } else if (move.move_type == MoveType::KingsideCastle || move.move_type == MoveType::QueensideCastle) {
      castle_cnt++;
    }
  }
  REQUIRE(capture_cnt == 8);
  REQUIRE(castle_cnt == 2);
}

TEST_CASE("test perft(3) kiwipete position", "[move_generator]") {
  MoveGenerator move_gen;
  std::string kp_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
  Position pos(kp_position);
  int move_count = move_gen.computePerft(pos, 3);
  REQUIRE(move_count == 97862);
}

TEST_CASE("test perft(4) position2 position", "[move_generator]") {
  MoveGenerator move_gen;
  std::string p2_position = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
  Position pos(p2_position);
  int move_count = move_gen.computePerft(pos, 4);
  REQUIRE(move_count == 43238);
}

TEST_CASE("test generateMoves() check position move count", "[move_generator]") {
  MoveGenerator move_gen;
  std::string check_position = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  Position pos(check_position);

  MoveVec moves = move_gen.generateMoves(pos);
  REQUIRE(moves.size() == 6);

  int capture_cnt = 0;
  int castle_cnt = 0;
  for (const auto& move : moves) {
    if (move.move_type == MoveType::Capture || move.move_type == MoveType::EnPassantCapture) {
      capture_cnt++;
    } else if (move.move_type == MoveType::KingsideCastle || move.move_type == MoveType::QueensideCastle) {
      castle_cnt++;
    }
  }
  REQUIRE(capture_cnt == 0);
  REQUIRE(castle_cnt == 0);
}

TEST_CASE("test generateMoves() position 6 move count", "[move_generator]") {
  MoveGenerator move_gen;
  std::string position_6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
  Position pos(position_6);

  MoveVec moves = move_gen.generateMoves(pos);
  REQUIRE(moves.size() == 46);
}

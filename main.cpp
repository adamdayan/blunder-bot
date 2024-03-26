#include "bitboard.h"
#include "net.h"
#include "stdio.h"

#include "position.h"
#include "constants.h"
#include "useful_fens.h"
#include "move_generator.h"
#include "zobrist_hash.h"
#include "search.h"
#include "net.h"


int main() {
  // // BitBoard demo
  // BitBoard bb(1502021);
  // bb.print();

  

  // // Pawn push demo
  // Position pos("1b1k4/8/8/3r4/3PP3/8/6PP/6K1 w - -");
  // printf("Position\n");
  // pos.print();
  // BoardPerspective persp(pos.getSideToMove());
  // MoveGenerator gen;
  // printf("Pawn pushes\n");
  // BitBoard sp = gen.computeSinglePawnPushes(pos, persp);
  // sp.print();
  // gen.computeDoublePawnPushes(pos, persp, sp).print();
  // printf("Pawn captures\n");
  // gen.computeWestPawnCaptures(pos, persp, pos.getPieceBitBoard(persp.side_to_move, PieceType::Pawn), pos.getPieceBitBoard(persp.opponent, PieceType::All)).print();



  // //  move gen demo
  // ZobristHash::initialiseKeys();

  // MoveGenerator move_gen;
  // std::string init_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  // Position pos(init_position);
  // int move_count = move_gen.computePerft(pos, 4);
  // pos.print();
  // printf("move_count: %d\n\n\n", move_count);

  // std::string kp_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
  // Position kp_pos(kp_position);
  // int kp_move_count = move_gen.computePerft(kp_pos, 4);
  // kp_pos.print();
  // printf("move_count: %d\n", kp_move_count);



  // search demo
  ZobristHash::initialiseKeys();
  BlunderNet net("/home/adam/dev/blunder-bot/python/scripted_supervised_learning_model.pt");
  GumbelMCTS searcher(&net, 10);

  Position pos("6k1/5ppp/p7/P7/5b2/7P/1r3PP1/3R2K1 w - - 0 1");
  pos.print();
  Move best_move = searcher.getBestMove(pos);
  best_move.print(pos);
  printf("\n\n\n");

  Position wrong_pos("r1bqkb1r/pppp1ppp/2n2n2/3Q4/2B1P3/8/PB3PPP/RN2K1NR w KQkq - 0 1");
  wrong_pos.print();
  Move wrong_move = searcher.getBestMove(wrong_pos);
  wrong_move.print(wrong_pos);

  return 0;
}
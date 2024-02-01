#include "stdio.h"

#include "position.h"
#include "constants.h"
#include "useful_fens.h"
#include "move_generator.h"
#include "zobrist_hash.h"


int main() {
  ZobristHash::initialiseKeys();

  MoveGenerator move_gen;
  std::string  kp_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

  Position pos(kp_position);
  move_gen.dividePerft(pos, 3); 

  return 0;
}
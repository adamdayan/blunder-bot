#include "stdio.h"

#include "position.h"
#include "constants.h"
#include "useful_fens.h"
#include "move_generator.h"


int main() {
  MoveGenerator move_gen;
  std::string p2_position = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";

  Position pos(p2_position);
  move_gen.dividePerft(pos, 4); 

  return 0;
}
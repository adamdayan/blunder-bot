#include "stdio.h"

#include <position.h>
#include <constants.h>


int main() {
  BitBoard bb;
  bb.setBoard(2);
  printf("bb.getLowestSetBit(%llu) = %d\n", bb.board, bb.getLowestSetBit()); 
  printf("bb.getHighestSetBit(%llu) = %d\n", bb.board, bb.getHighestSetBit()); 

  bb.setBoard(1);
  printf("bb.getLowestSetBit(%llu) = %d\n", bb.board, bb.getLowestSetBit()); 
  printf("bb.getHighestSetBit(%llu) = %d\n", bb.board, bb.getHighestSetBit()); 
  bb.print();

  bb.clear();
  bb.setBit(6, 6);
  bb.print();

  Position pos;
  pos.print();

  pos.parseFEN(tricky_position);
  pos.print();

  pos.parseFEN(empty_board);
  pos.print();

  return 0;
}
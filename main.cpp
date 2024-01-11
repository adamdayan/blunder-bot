#include "stdio.h"
#include <board.h>


int main() {
  BitBoard bb;
  bb.setBoard(2);
  printf("bb.getLowestSetBit(%llu) = %d\n", bb.board, bb.getLowestSetBit()); 
  printf("bb.getHighestSetBit(%llu) = %d\n", bb.board, bb.getHighestSetBit()); 

  bb.setBoard(1);
  printf("bb.getLowestSetBit(%llu) = %d\n", bb.board, bb.getLowestSetBit()); 
  printf("bb.getHighestSetBit(%llu) = %d\n", bb.board, bb.getHighestSetBit()); 

  return 0;
}
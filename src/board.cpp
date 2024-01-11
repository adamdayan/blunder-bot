#include "board.h"

int BitBoard::getLowestSetBit() {
  return __builtin_ffsll(board);
}

int BitBoard::getHighestSetBit() {
  // TODO: check this works
  return 64 - __builtin_clz(board);
}

int BitBoard::popLowestSetBit() {
  int set_bit = getLowestSetBit();
  clearBit(set_bit);
  return set_bit;
}

int BitBoard::popHighestSetBit() {
  int set_bit = getHighestSetBit();
  clearBit(set_bit);
  return set_bit;
}

void BitBoard::clearBit(int bit_index) {
  board = board ^ (static_cast<uint64_t>(1) << (bit_index -1));
}


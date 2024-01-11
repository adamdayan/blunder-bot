#include <bit>

#include "board.h"

void BitBoard::setBoard(uint64_t val) {
  board = val;
}

int BitBoard::getLowestSetBit() {
  int bit_index = std::countr_zero(board);
  // NOTE: decide if this is the correct way to handle there being 0 set bits
  if (bit_index == 64) return -1;
  return bit_index;
}

int BitBoard::getHighestSetBit() {
  return 63 - std::countl_zero(board);
}

int BitBoard::popLowestSetBit() {
  int bit_index = getLowestSetBit();
  if (bit_index >= 0) {
    clearBit(bit_index);
  }
  return bit_index;
}

int BitBoard::popHighestSetBit() {
  int bit_index = getHighestSetBit();
  if (bit_index >= 0) {
    clearBit(bit_index);
  }
  return bit_index;
}

void BitBoard::clearBit(int bit_index) {
  board = board ^ (static_cast<BoardBits>(1) << (bit_index));
}


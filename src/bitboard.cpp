#include <bit>

#include "bitboard.h"
#include "utils.h"

void BitBoard::setBoard(BoardBits val) {
  board = val;
}

void BitBoard::setBit(int bit_index) {
  board |= (static_cast<BoardBits>(1) << bit_index);
}

void BitBoard::setBit(int rank, int file) {
  int bit_index = rankFileToIndex(rank, file);
  setBit(bit_index);
}

bool BitBoard::getBit(int bit_index) const {
  return static_cast<BoardBits>(1) & (board >> bit_index);
}

bool BitBoard::getBit(int rank, int file) const {
  int bit_index = rankFileToIndex(rank, file);
  return getBit(bit_index);
}

bool BitBoard::isEmpty() const {
  return countSetBits() == 0;
}

BitBoard BitBoard::shift(Direction dir) const {
  // TODO: write tests for this
  if (dir == Direction::North) {
    return BitBoard(board << 8);
  } else if (dir == Direction::NorthEast) {
    // NOTE: will this work?
    return shift(Direction::North).shift(Direction::East);
  } else if (dir == Direction::East) {
    // we don't want to shift pieces on the H-file east because they will wrap
    // around
    return BitBoard((board & ~FILEH)<< 1);
  } else if (dir == Direction::SouthEast) {
    return shift(Direction::South).shift(Direction::East);
  } else if (dir == Direction::South) {
    return BitBoard(board >> 8);
  } else if (dir == Direction::SouthWest) {
    return shift(Direction::South).shift(Direction::West);
  } else if (dir == Direction::West) {
    return BitBoard((board & ~FILEA) >> 1);
  } else if (dir == Direction::NorthWest) {
    return shift(Direction::North).shift(Direction::West);
  }
  // TODO: decide how to handle bad input
  return BitBoard();
}

int BitBoard::getLowestSetBit() const {
  int bit_index = std::countr_zero(board);
  // NOTE: decide if this is the correct way to handle there being 0 set bits
  if (bit_index == 64) return -1;
  return bit_index;
}

int BitBoard::getHighestSetBit() const {
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

int BitBoard::countSetBits() const {
  return std::popcount(board);
}

void BitBoard::clearBit(int bit_index) {
  board = board ^ (static_cast<BoardBits>(1) << (bit_index));
}

void BitBoard::clearBitsAbove(int bit_index) {
  BoardBits mask;
  if (bit_index == 64) {
    // required because (1 << 64) wraps around to 1 not 0
    mask = BoardBits(0) - 1;
  } else {
    mask = (BoardBits(1) << bit_index) - 1;
  }
  board = board & mask;
}

void BitBoard::clearBitsBelow(int bit_index) {
  BoardBits mask = ~((BoardBits(1) << (bit_index + 1)) - 1);
  board = board & mask;
}

void BitBoard::clear() {
  setBoard(0);
}

BitBoard BitBoard::operator&(const BitBoard& bb) const {
  return BitBoard(board & bb.board);
}

BitBoard BitBoard::operator|(const BitBoard& bb) const {
  return BitBoard(board | bb.board);
}

BitBoard BitBoard::operator^(const BitBoard& bb) const {
  return BitBoard(board ^ bb.board);
}

BitBoard BitBoard::operator~() const {
  return BitBoard(~board);
}

BitBoard BitBoard::operator<<(int n) const {
  return BitBoard(board << n);
}

BitBoard BitBoard::operator>>(int n) const {
  return BitBoard(board >> n);
}

void BitBoard::operator&=(const BitBoard& bb) {
  board &= bb.board;
}

void BitBoard::operator|=(const BitBoard& bb) {
  board |= bb.board;
}

void BitBoard::operator^=(const BitBoard& bb) {
  board ^= bb.board;
}


void BitBoard::print() const {
  // iterate through each rank, if there's a piece on a file then print it
  // otherwise print blank
  for (int rank = 7; rank >= 0; rank--) {
    std::string rank_str = std::to_string(rank + 1) + " ";
    for (int file = 0; file < 8; file++) {
      if (getBit(rank, file)) {
        // const maps don't allow use of [] so must use find instead
        rank_str += std::to_string(1);
      } else {
        rank_str += std::to_string(0);
      }
      rank_str += " ";
    }
    printf("%s\n", rank_str.c_str());
  }
  std::string file_name_str = "  ";
  for (const std::string& file : file_names) {
    file_name_str += file + " ";
  }
  printf("%s \n", file_name_str.c_str());
  printf("\n");
}
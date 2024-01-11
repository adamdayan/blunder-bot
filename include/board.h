#include <cstdint>

// NOTE: is aliasing actually useful?
using BoardBits = unsigned long long;

// holds an aspect of positional board state in a 64-bit uint
class BitBoard {
  public:
    BitBoard() {};
    BitBoard(BoardBits val) : board(val) {};

    void setBoard(uint64_t val);

    // returns lowest set bit on the bitboard which corresponds to squares closer to a1
    // returns. returns -1 if no set bits 
    int getLowestSetBit();
    // returns highest set bit on the bitboard which corresponds to squares closer to h8. 
    // returns -1 if no bits set
    int getHighestSetBit();
    // returns and clears lowest set bit
    int popLowestSetBit();
    // returns and clears highest set bit
    int popHighestSetBit();
    // clears bit
    void clearBit(int bit_index);

    BoardBits board = 0;
};

// class Board {

// };
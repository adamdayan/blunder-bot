#include <cstdint>

// holds an aspect of positional board state in a 64-bit uint
class BitBoard {
  public:
    BitBoard() {};
    // returns lowest set bit on the bitboard which corresponds to squares closer to a1
    int getLowestSetBit();
    // returns highest set bit on the bitboard which corresponds to squares closer to h8
    int getHighestSetBit();
    // returns and clears lowest set bit
    int popLowestSetBit();
    // returns and clears highest set bit
    int popHighestSetBit();
    // clears 1-indexed bit
    void clearBit(int bit_index);

    uint64_t board;
};

// class Board {

// };
#ifndef BITBOARD_H
#define BITBOARD_H

#include "constants.h"

// holds an aspect of positional board state in a 64-bit uint
class BitBoard {
  public:
    BitBoard() {};
    BitBoard(BoardBits val) : board(val) {};

    // sets BoardBits value
    void setBoard(BoardBits val);

    // sets bit specified by index
    void setBit(int bit_index);
    // sets bit specified by rank and file;
    void setBit(int rank, int file);

    // gets bit specificied by index
    bool getBit(int bit_index) const;
    // gets bits specified by rank and file
    bool getBit(int rank, int file) const;

    // returns true if no bits sets
    bool isEmpty() const;

    // returns a new bitboard shifted in the specified direction
    BitBoard shift(Direction dir) const;

    // returns a new bitboard reflected along the vertical axis
    BitBoard flip() const;

    // returns lowest set bit index on the bitboard which corresponds to squares closer to a1
    // returns. returns -1 if no set bits 
    int getLowestSetBit() const;
    // returns highest set bit index on the bitboard which corresponds to squares closer to h8. 
    // returns -1 if no bits set
    int getHighestSetBit() const;
    // returns index of and clears lowest set bit
    int popLowestSetBit();
    // returns index of and clears highest set bit
    int popHighestSetBit();

    // counts set bits
    int countSetBits() const;

    // clears bit
    void clearBit(int bit_index);
    // clears all bits higher at bit_index or higher
    void clearBitsAbove(int bit_index);
    // clear all bits at bit index or lower
    void clearBitsBelow(int bit_index);
    // sets bitboard to 0
    void clear();

    // overload bitwise operators to operate on underlying BoardBits 
    BitBoard operator&(const BitBoard& bb) const;
    BitBoard operator|(const BitBoard& bb) const;
    BitBoard operator~() const;
    BitBoard operator^(const BitBoard& bb) const;

    void operator&=(const BitBoard& bb);
    void operator|=(const BitBoard& bb);
    void operator^=(const BitBoard& bb);

    BitBoard operator<<(int n) const;
    BitBoard operator>>(int n) const;

    // prints out the bitboard, optionally takes piece type otherwise defaults
    // to "*"
    void print() const;

    BoardBits board = 0;
};

#endif // BITBOARD_H
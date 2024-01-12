#ifndef POSITION_H 
#define POSITION_H 

#include <array>

#include "constants.h"

// NOTE: is aliasing this actually useful?
using BoardBits = unsigned long long;

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

    bool getBit(int bit_index) const;
    bool getBit(int rank, int file) const;

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

    // prints out the bitboard, optionally takes piece type otherwise defaults
    // to "*"
    void print(Piece piece_type = Piece::All) const;

    BoardBits board = 0;
};

class Position {
  public:
    // sets up board in initial configuration
    Position();

    // prints out position
    void print() const;

  private:
    // NOTE: is this double array + member more horrible than 1 enum with all
    // options?
    std::array<std::array<BitBoard, 7>, 2> bit_boards; 
    BitBoard all_pieces;

};

#endif // POSITION_H 
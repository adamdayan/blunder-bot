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

    // returns lowest set bit index on the bitboard which corresponds to squares closer to a1
    // returns. returns -1 if no set bits 
    int getLowestSetBit();
    // returns highest set bit index on the bitboard which corresponds to squares closer to h8. 
    // returns -1 if no bits set
    int getHighestSetBit();
    // returns index of and clears lowest set bit
    int popLowestSetBit();
    // returns index of and clears highest set bit
    int popHighestSetBit();
    // clears bit
    void clearBit(int bit_index);

    // sets bitboard to 0
    void clear();

    // prints out the bitboard, optionally takes piece type otherwise defaults
    // to "*"
    void print() const;

    BoardBits board = 0;
};

class Position {
  public:
    // initialises Position, uses start position as default
    Position(const std::string& fen = start_position);

    // sets all bitboards to 0
    void clear();

    // reads FEN string and sets Position to the according position
    void parseFEN(const std::string& fen);

    // prints out position
    void print() const;


  private:
    // NOTE: is this double array + member more horrible than 1 enum with all
    // options?
    std::array<std::array<BitBoard, 7>, 2> bit_boards; 
    BitBoard all_pieces;
    BitBoard enpassant;
    Colour side_to_move;
    std::array<std::array<bool, 2>, 2> castling_rights;
    int halfmove_clock = 0;
    int fullmove_cnt = 0;

};

#endif // POSITION_H 
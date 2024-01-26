#ifndef POSITION_H 
#define POSITION_H 

#include <array>

#include "constants.h"
#include "useful_fens.h"

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

    // getters
    // NOTE: very unsure whether bit boards should be public?? Concerned
    // MoveGenerator breaks encapsulation. TBD! Also, should these return const references?
    BitBoard getPieceBitBoard(Colour colour, PieceType piece_type) const;
    std::array<BitBoard, 7> getPieceBitBoardsByColour(Colour colour) const;
    BitBoard getAllPiecesBitBoard() const;
    BitBoard getEnpassantBitBoard() const;
    Colour getSideToMove() const;
    bool canCastle(Colour colour, CastlingType castling_type) const;

    // methods needed for detecting draws
    bool isDrawBy50Moves() const;
    // TODO: complete
    bool isDrawByRepetition() const;
    bool isDrawByInsufficientMaterial() const;

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
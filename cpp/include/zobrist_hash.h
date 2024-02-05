#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H

#include "constants.h"

class Position;

class ZobristHash {
  public:
    // executed once per run to initialise static keys
    static void initialiseKeys();

    ZobristHash() {}
    ZobristHash(const Position& pos);
    // updates hash to add/remove given piece of given colour from given square
    void updatePiece(Colour colour, PieceType piece_type, int square);
    // updates hash to change side to move
    void updateSide(Colour colour);
    // updates hash to add/remove an enpassant square
    void updateEnpassant(int square);
    // updates hash to add/remove given castling right
    void updateCastlingRights(Colour colour, CastlingType castling_type);

    // returns current hash
    unsigned long long getHash() const;


  private:
    unsigned long long val = 0;

    // TODO: it would be better if I could make these const
    // we should only create these once per run
    static std::array<std::array<std::array<unsigned long long, 64>, 6>, 2> piece_keys;
    static std::array<std::array<unsigned long long, 2>, 2> castling_rights_keys;
    static std::array<unsigned long long, 64> enpassant_keys;
    static std::array<unsigned long long, 2> colour_keys;
};

#endif // ZOBRIST_HASH_H
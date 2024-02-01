#include <random>

#include "zobrist_hash.h"
#include "constants.h"
#include "position.h"

std::array<std::array<std::array<unsigned long long, 64>, 6>, 2> ZobristHash::piece_keys;
std::array<std::array<unsigned long long, 2>, 2> ZobristHash::castling_rights_keys;
std::array<unsigned long long, 64> ZobristHash::enpassant_keys;
std::array<unsigned long long, 2> ZobristHash::colour_keys;

void ZobristHash::initialiseKeys() {
  std::random_device rd;
  std::mt19937_64 twister(rd());
  std::uniform_int_distribution<unsigned long long> dist;
  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = 0; piece <= PieceType::King; piece++) {
      for (int square = 0; square < 64; square++) {
        piece_keys[colour][piece][square] = dist(twister);
      }
    }
  }

  for (int square = 0; square < 64; square++) {
    enpassant_keys[square] = dist(twister);
  }

  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    colour_keys[colour] = dist(twister);
    for (int castling_type = CastlingType::Kingside; castling_type <= CastlingType::Queenside; castling_type++) {
      castling_rights_keys[colour][castling_type] = dist(twister);
    }
  }
}

ZobristHash::ZobristHash(const Position& pos) {
  // hash all piece positions
  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = 0; piece <= PieceType::King; piece++) {
      BitBoard bb = pos.getPieceBitBoard(static_cast<Colour>(colour), static_cast<PieceType>(piece));
      while (!bb.isEmpty()) {
        int square_bit_index = bb.popHighestSetBit();
        updatePiece(static_cast<Colour>(colour), static_cast<PieceType>(piece), square_bit_index);
      }
    }
  }
  // hash enpassant
  BitBoard enpassant_bb = pos.getEnpassantBitBoard();
  while (!enpassant_bb.isEmpty()) {
    int square_bit_index = enpassant_bb.popHighestSetBit();
    updateEnpassant(square_bit_index);
  }

  // hash castling rights
  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int castling_type = CastlingType::Kingside; castling_type <= CastlingType::Queenside; castling_type++) {
      if (pos.canCastle(static_cast<Colour>(colour), static_cast<CastlingType>(castling_type))) {
        updateCastlingRights(static_cast<Colour>(colour), static_cast<CastlingType>(castling_type));
      }
    }
  }

  updateSide(pos.getSideToMove());
}

void ZobristHash::updatePiece(Colour colour, PieceType piece_type, int square) {
  val ^= piece_keys[colour][piece_type][square];
}

void ZobristHash::updateSide(Colour colour) {
  val ^= colour_keys[colour];
}

void ZobristHash::updateEnpassant(int square) {
  val ^= enpassant_keys[square];
}

void ZobristHash::updateCastlingRights(Colour colour, CastlingType castling_type) {
  val ^= castling_rights_keys[colour][castling_type];
}

unsigned long long ZobristHash::getHash() const {
  return val;
}
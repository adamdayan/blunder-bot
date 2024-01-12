#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
#include <unordered_map>
#include <string>

enum Piece {
  Pawn,
  Knight,
  Bishop,
  Rook,
  Queen,
  King,
  All,
};

enum Colour {
  White,
  Black,
};

const std::unordered_map<Piece, std::string> piece_to_string = {
  {Piece::Pawn, "P"},
  {Piece::Knight, "N"},
  {Piece::Bishop, "B"},
  {Piece::Rook, "R"},
  {Piece::Queen, "Q"},
  {Piece::King, "K"},
  {Piece::All, "*"},
};

const std::array<std::string, 8> file_names = {"A", "B", "C", "D", "E", "F", "G", "H"};

#endif // CONSTANTS_H
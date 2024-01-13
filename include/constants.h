#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
#include <unordered_map>
#include <string>

enum PieceType {
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

enum RayDirection {
  North,
  NorthEast,
  East,
  SouthEast,
  South,
  SouthWest,
  West,
  NorthWest,
};

enum CastlingType {
  Kingside,
  Queenside
};

const std::array<std::array<std::string, 6>, 2> piece_to_string = {{
    {{"P", "N", "B", "R", "Q", "K"}},
    {{"p", "n", "b", "r", "q", "k"}},
}};

// NOTE: is it bad that the piece2string and string2piece maps are not the same
// type?
const std::unordered_map<std::string, std::pair<Colour, PieceType>> string_to_piece = {
  {"P", {Colour::White, PieceType::Pawn}},
  {"N", {Colour::White, PieceType::Knight}},
  {"B", {Colour::White, PieceType::Bishop}},
  {"R", {Colour::White, PieceType::Rook}},
  {"Q", {Colour::White, PieceType::Queen}},
  {"K", {Colour::White, PieceType::King}},

  {"p", {Colour::Black, PieceType::Pawn}},
  {"n", {Colour::Black, PieceType::Knight}},
  {"b", {Colour::Black, PieceType::Bishop}},
  {"r", {Colour::Black, PieceType::Rook}},
  {"q", {Colour::Black, PieceType::Queen}},
  {"k", {Colour::Black, PieceType::King}},
};

// NOTE: it's possible these are the wrong way around and my VSCode is rendering
// them weirdly
const std::array<std::array<std::string, 6>, 2> piece_to_pretty_string = {{
  {{"♟", "♞", "♝", "♜", "♛", "♚"}},
  {{"♙", "♘", "♗", "♖", "♕", "♔"}}
}};

const std::array<std::string, 8> file_names = {"a", "b", "c", "d",
                                               "e", "f", "g", "h"};

// some useful FEN-encoded positions
const std::string empty_board = "8/8/8/8/8/8/8/8 w - - ";
const std::string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
const std::string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
const std::string enpassant_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq e4 0 1 ";


#endif // CONSTANTS_H
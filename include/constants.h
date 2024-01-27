#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
#include <unordered_map>
#include <string>

// NOTE: is aliasing this actually useful?
using BoardBits = unsigned long long;

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

enum Direction {
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

enum MoveType {
  Quiet,
  Capture,
  EnPassantCapture,
  KingsideCastle,
  QueensideCastle,
  // TODO: add promotion?
};


// 1-indexed like on the chess board
constexpr BoardBits RANK1 = 0xFF;
constexpr BoardBits RANK2 = RANK1 << 8;
constexpr BoardBits RANK3 = RANK1 << (8 * 2);
constexpr BoardBits RANK4 = RANK1 << (8 * 3);
constexpr BoardBits RANK5 = RANK1 << (8 * 4);
constexpr BoardBits RANK6 = RANK1 << (8 * 5);
constexpr BoardBits RANK7 = RANK1 << (8 * 6);
constexpr BoardBits RANK8 = RANK1 << (8 * 7);

constexpr BoardBits FILEA = 72340172838076673;
constexpr BoardBits FILEB = FILEA << 1;
constexpr BoardBits FILEC = FILEA << 2;
constexpr BoardBits FILED = FILEA << 3;
constexpr BoardBits FILEE = FILEA << 4;
constexpr BoardBits FILEF = FILEA << 5;
constexpr BoardBits FILEG = FILEA << 6;
constexpr BoardBits FILEH = FILEA << 7;

const std::array<std::string, 8> file_to_name = {"A", "B", "C", "D", "E", "F", "G", "H"};

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

// squares the king must move through to castle. indexed by (Colour, CastlingType)
const std::array<std::unordered_map<MoveType, BoardBits>,2> CASTLE_MASKS = {{
  {{MoveType::KingsideCastle, 96}, {MoveType::QueensideCastle, 12}},
  {{MoveType::KingsideCastle, 6917529027641081856}, {MoveType::QueensideCastle, 864691128455135232}}
}};

// NOTE: it's possible these colours are the wrong way around and my VSCode is rendering
// them weirdly
const std::array<std::array<std::string, 6>, 2> piece_to_pretty_string = {{
  {{"♟", "♞", "♝", "♜", "♛", "♚"}},
  {{"♙", "♘", "♗", "♖", "♕", "♔"}}
}};

const std::array<std::string, 8> file_names = {"a", "b", "c", "d",
                                               "e", "f", "g", "h"};

// constant numbers
constexpr int MAX_HALFMOVE_CNT = 100;

constexpr int SINGLE_PAWN_PUSH_OFFSET = 8;
constexpr int DOUBLE_PAWN_PUSH_OFFSET = 16;

constexpr int WHITE_PAWN_EAST_CAPTURE_OFFSET = 9;
constexpr int WHITE_PAWN_WEST_CAPTURE_OFFSET = 7;
constexpr int BLACK_PAWN_EAST_CAPTURE_OFFSET = -7;
constexpr int BLACK_PAWN_WEST_CAPTURE_OFFSET = -9;

#endif // CONSTANTS_H
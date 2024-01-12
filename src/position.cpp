#include <bit>

#include "position.h"
#include "constants.h"

void BitBoard::setBoard(BoardBits val) {
  board = val;
}

void BitBoard::setBit(int bit_index) {
  board |= (static_cast<BoardBits>(1) << bit_index);
}

void BitBoard::setBit(int rank, int file) {
  int bit_index = (rank * 8) + file;
  setBit(bit_index);
}

bool BitBoard::getBit(int bit_index) const {
  return static_cast<BoardBits>(1) & (board >> bit_index);
}

bool BitBoard::getBit(int rank, int file) const {
  int bit_index = (rank * 8) + file;
  return getBit(bit_index);
}

int BitBoard::getLowestSetBit() {
  int bit_index = std::countr_zero(board);
  // NOTE: decide if this is the correct way to handle there being 0 set bits
  if (bit_index == 64) return -1;
  return bit_index;
}

int BitBoard::getHighestSetBit() {
  return 63 - std::countl_zero(board);
}

int BitBoard::popLowestSetBit() {
  int bit_index = getLowestSetBit();
  if (bit_index >= 0) {
    clearBit(bit_index);
  }
  return bit_index;
}

int BitBoard::popHighestSetBit() {
  int bit_index = getHighestSetBit();
  if (bit_index >= 0) {
    clearBit(bit_index);
  }
  return bit_index;
}

void BitBoard::clearBit(int bit_index) {
  board = board ^ (static_cast<BoardBits>(1) << (bit_index));
}

void BitBoard::print(const Piece piece_type) const {
  for (int rank = 7; rank >= 0; rank--) {
    std::string rank_str = std::to_string(rank + 1) + " ";
    for (int file = 0; file < 8; file++) {
      if (getBit(rank, file)) {
        // TODO: why doesn't [] operator work here?
        rank_str += piece_to_string.find(piece_type)->second;
      } else {
        rank_str += " ";
      }
      rank_str += " ";
    }
    printf("%s\n", rank_str.c_str());
  }
  std::string file_name_str = "  ";
  for (const std::string& file : file_names) {
    file_name_str += file + " ";
  }
  printf("%s \n", file_name_str.c_str());
}

Position::Position() {
  // pawns
  for (int i = 0; i < 8; i++) {
    bit_boards[Colour::White][Piece::Pawn].setBit(1, i);
    bit_boards[Colour::Black][Piece::Pawn].setBit(6, i);
  }

  // knights
  bit_boards[Colour::White][Piece::Knight].setBit(0, 1);
  bit_boards[Colour::White][Piece::Knight].setBit(0, 6);

  bit_boards[Colour::Black][Piece::Knight].setBit(7, 1);
  bit_boards[Colour::Black][Piece::Knight].setBit(7, 6);

  // bishops
  bit_boards[Colour::White][Piece::Bishop].setBit(0, 2);
  bit_boards[Colour::White][Piece::Bishop].setBit(0, 5);

  bit_boards[Colour::Black][Piece::Bishop].setBit(7, 2);
  bit_boards[Colour::Black][Piece::Bishop].setBit(7, 5);

  // rooks
  bit_boards[Colour::White][Piece::Rook].setBit(0, 0);
  bit_boards[Colour::White][Piece::Rook].setBit(0, 7);

  bit_boards[Colour::Black][Piece::Rook].setBit(7, 0);
  bit_boards[Colour::Black][Piece::Rook].setBit(7, 7);

  // queens 
  bit_boards[Colour::White][Piece::Queen].setBit(0, 3);
  bit_boards[Colour::Black][Piece::Queen].setBit(7, 3);

  // kings
  bit_boards[Colour::White][Piece::King].setBit(0, 4);
  bit_boards[Colour::Black][Piece::King].setBit(7, 4);

  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = Piece::Pawn; piece <= Piece::King; piece++) {
      // TODO: if it proves worthwhile, operator overload all of BitBoard's
      // bitwise operators to apply to the underlying BoardBits
      all_pieces.board |= bit_boards[colour][piece].board;
      bit_boards[colour][Piece::All].board |= bit_boards[colour][piece].board;
    }
  }
}

void Position::print() const {

  // NOTE: not great that this repeats the code from BitBoard
  for (int rank = 7; rank >= 0; rank--) {
    std::string rank_str = std::to_string(rank + 1) + " ";
    for (int file = 0; file < 8; file++) {

      for (int colour = Colour::White; colour <= Colour::Black; colour++) {
        for (int piece = Piece::Pawn; piece < Piece::All; piece++) {
          const BitBoard& bb = bit_boards[colour][piece];
          // TODO: maybe want to use colour codes to differentiate sides
          if (bb.getBit(rank, file)) {
            rank_str += piece_to_string.find(static_cast<Piece>(piece))->second + " ";
          } else if (!all_pieces.getBit(rank, file) && piece == Piece::Pawn) {
            rank_str += "  ";
          }
        }
      }

    }
    printf("%s\n", rank_str.c_str());
  }

  std::string file_name_str = "  ";
  for (const std::string& file : file_names) {
    file_name_str += file + " ";
  }
  printf("%s \n", file_name_str.c_str());
}
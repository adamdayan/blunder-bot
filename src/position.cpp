#include <bit>

#include "position.h"
#include "constants.h"
#include "utils.h"

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

void BitBoard::clear() {
  setBoard(0);
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

void BitBoard::print() const {
  // iterate through each rank, if there's a piece on a file then print it
  // otherwise print blank
  for (int rank = 7; rank >= 0; rank--) {
    std::string rank_str = std::to_string(rank + 1) + " ";
    for (int file = 0; file < 8; file++) {
      if (getBit(rank, file)) {
        // const maps don't allow use of [] so must use find instead
        rank_str += std::to_string(1);
      } else {
        rank_str += std::to_string(0);
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
  printf("\n");
}

void Position::clear() {
  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = PieceType::Pawn; piece <= PieceType::All; piece++) {
      bit_boards[colour][piece].clear();
    }
  }

  all_pieces.clear();
}

void Position::parseFEN(const std::string& fen) {
  clear();
  int fen_idx = 0;
  int rank = 7;
  int file = 0;
  // TODO: add parsing of post position FEN
  while (rank >= 0 && file < 8) {
    char cur_char = fen[fen_idx];
    if ((cur_char >= 'a' && cur_char <= 'z') ||
        (cur_char >= 'A' && cur_char <= 'Z')) {
      std::pair<Colour, PieceType> piece_info =
          string_to_piece.find(std::string(1, cur_char))->second;
      
      // set piece-specific and "All" bitboards
      // NOTE: is it worth combining these 3 operations into a "placePiece" method?
      bit_boards[piece_info.first][piece_info.second].setBit(rank, file);
      bit_boards[piece_info.first][PieceType::All].setBit(rank, file);
      all_pieces.setBit(rank, file);
      fen_idx++;
      file++;
      if (file > 7) {
        file = 0;
        rank--;
      }
    } else if (cur_char >= '1' && cur_char <= '8') {
      int offset = cur_char - '0';
      file += offset;
      if (file > 7) {
        file = 0;
        rank--;
      }
      fen_idx++;
    } else if (cur_char == '/') {
      fen_idx++;
    }
  }


  // // TODO: turn into while loop which makes way more sense
  // for (int rank = 7; rank >= 0; rank--) {
  //   for (int file = 0; file < 8; file++) {
  //     char cur_char = fen[fen_idx];
  //     if ((cur_char >= 'a' && cur_char <= 'z') ||
  //         (cur_char >= 'A' && cur_char <= 'Z')) {
  //       std::pair<Colour, PieceType> piece_info =
  //           string_to_piece.find(std::string(1, cur_char))->second;
        
  //       // set piece-specific and "All" bitboards
  //       // NOTE: is it worth combining these 3 operations into a "placePiece" method?
  //       bit_boards[piece_info.first][piece_info.second].setBit(rank, file);
  //       bit_boards[piece_info.first][PieceType::All].setBit(rank, file);
  //       all_pieces.setBit(rank, file);
  //       fen_idx++;
  //     } else if (cur_char >= '1' && cur_char <= '8') {
  //       int offset = cur_char - '0' - 1;
  //       file += offset;
  //       fen_idx++;
  //     } else if (cur_char == '/') {
  //       fen_idx++;
  //       file--;
  //     }
  //   }
  // }
}

Position::Position() {
  // pawns
  for (int i = 0; i < 8; i++) {
    bit_boards[Colour::White][PieceType::Pawn].setBit(1, i);
    bit_boards[Colour::Black][PieceType::Pawn].setBit(6, i);
  }

  // knights
  bit_boards[Colour::White][PieceType::Knight].setBit(0, 1);
  bit_boards[Colour::White][PieceType::Knight].setBit(0, 6);

  bit_boards[Colour::Black][PieceType::Knight].setBit(7, 1);
  bit_boards[Colour::Black][PieceType::Knight].setBit(7, 6);

  // bishops
  bit_boards[Colour::White][PieceType::Bishop].setBit(0, 2);
  bit_boards[Colour::White][PieceType::Bishop].setBit(0, 5);

  bit_boards[Colour::Black][PieceType::Bishop].setBit(7, 2);
  bit_boards[Colour::Black][PieceType::Bishop].setBit(7, 5);

  // rooks
  bit_boards[Colour::White][PieceType::Rook].setBit(0, 0);
  bit_boards[Colour::White][PieceType::Rook].setBit(0, 7);

  bit_boards[Colour::Black][PieceType::Rook].setBit(7, 0);
  bit_boards[Colour::Black][PieceType::Rook].setBit(7, 7);

  // queens 
  bit_boards[Colour::White][PieceType::Queen].setBit(0, 3);
  bit_boards[Colour::Black][PieceType::Queen].setBit(7, 3);

  // kings
  bit_boards[Colour::White][PieceType::King].setBit(0, 4);
  bit_boards[Colour::Black][PieceType::King].setBit(7, 4);

  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = PieceType::Pawn; piece <= PieceType::King; piece++) {
      // TODO: if it proves worthwhile, operator overload all of BitBoard's
      // bitwise operators to apply to the underlying BoardBits
      all_pieces.board |= bit_boards[colour][piece].board;
      bit_boards[colour][PieceType::All].board |= bit_boards[colour][piece].board;
    }
  }
}

void Position::print() const {
  // NOTE: not great that this repeats the code from BitBoard
  for (int rank = 7; rank >= 0; rank--) {
    std::string rank_str = std::to_string(rank + 1) + " ";
    for (int file = 0; file < 8; file++) {

      for (int colour = Colour::White; colour <= Colour::Black; colour++) {
        for (int piece = PieceType::Pawn; piece < PieceType::All; piece++) {
          const BitBoard& bb = bit_boards[colour][piece];
          if (bb.getBit(rank, file)) {
            rank_str += piece_to_pretty_string[colour][piece] + " ";
          } else if (!all_pieces.getBit(rank, file) &&
                     piece == PieceType::Pawn && colour == Colour::White) {
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
  printf("\n");
}
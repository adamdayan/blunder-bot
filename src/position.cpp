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
  int bit_index = rankFileToIndex(rank, file);
  setBit(bit_index);
}

bool BitBoard::getBit(int bit_index) const {
  return static_cast<BoardBits>(1) & (board >> bit_index);
}

bool BitBoard::getBit(int rank, int file) const {
  int bit_index = rankFileToIndex(rank, file);
  return getBit(bit_index);
}

bool BitBoard::isEmpty() const {
  return countSetBits() == 0;
}

BitBoard BitBoard::shift(Direction dir) const {
  // TODO: write tests for this
  if (dir == Direction::North) {
    return BitBoard(board << 8);
  } else if (dir == Direction::NorthEast) {
    // NOTE: will this work?
    return shift(Direction::North).shift(Direction::East);
  } else if (dir == Direction::East) {
    // we don't want to shift pieces on the H-file east because they will wrap
    // around
    return BitBoard((board & ~FILEH)<< 1);
  } else if (dir == Direction::SouthEast) {
    return shift(Direction::South).shift(Direction::East);
  } else if (dir == Direction::South) {
    return BitBoard(board >> 8);
  } else if (dir == Direction::SouthWest) {
    return shift(Direction::South).shift(Direction::West);
  } else if (dir == Direction::West) {
    return BitBoard((board & ~FILEA) >> 1);
  } else if (dir == Direction::NorthWest) {
    return shift(Direction::North).shift(Direction::West);
  }
  // TODO: decide how to handle bad input
  return BitBoard();
}

int BitBoard::getLowestSetBit() const {
  int bit_index = std::countr_zero(board);
  // NOTE: decide if this is the correct way to handle there being 0 set bits
  if (bit_index == 64) return -1;
  return bit_index;
}

int BitBoard::getHighestSetBit() const {
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

int BitBoard::countSetBits() const {
  return std::popcount(board);
}

void BitBoard::clearBit(int bit_index) {
  board = board ^ (static_cast<BoardBits>(1) << (bit_index));
}

void BitBoard::clearBitsAbove(int bit_index) {
  BoardBits mask;
  if (bit_index == 64) {
    // required because (1 << 64) wraps around to 1 not 0
    mask = BoardBits(0) - 1;
  } else {
    mask = (BoardBits(1) << bit_index) - 1;
  }
  board = board & mask;
}

void BitBoard::clearBitsBelow(int bit_index) {
  BoardBits mask = ~((BoardBits(1) << (bit_index + 1)) - 1);
  board = board & mask;
}

void BitBoard::clear() {
  setBoard(0);
}

BitBoard BitBoard::operator&(const BitBoard& bb) const {
  return BitBoard(board & bb.board);
}

BitBoard BitBoard::operator|(const BitBoard& bb) const {
  return BitBoard(board | bb.board);
}

BitBoard BitBoard::operator^(const BitBoard& bb) const {
  return BitBoard(board ^ bb.board);
}

BitBoard BitBoard::operator~() const {
  return BitBoard(~board);
}

BitBoard BitBoard::operator<<(int n) const {
  return BitBoard(board << n);
}

BitBoard BitBoard::operator>>(int n) const {
  return BitBoard(board >> n);
}

void BitBoard::operator&=(const BitBoard& bb) {
  board &= bb.board;
}

void BitBoard::operator|=(const BitBoard& bb) {
  board |= bb.board;
}

void BitBoard::operator^=(const BitBoard& bb) {
  board ^= bb.board;
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
  // TODO: defensively check the FEN-string is valid

  // parse position from FEN
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

  // parse side-to-move
  fen_idx++;
  if (fen[fen_idx] == 'w') {
    side_to_move = Colour::White;
  } else {
    side_to_move = Colour::Black;
  }
  fen_idx+= 2;

  // parse castling rights
  castling_rights[Colour::White].fill(false);
  castling_rights[Colour::Black].fill(false);

  // TODO: this is an insufficient handling of incomplete FEN strings.
  // We should probably parse FENs which include only board position but no
  // metadata but we need some guard against then trying to use the resultant
  // semi-incomplete position
  if (fen_idx >= fen.size() || fen_idx == fen.size() -1) {
    return;
  }

  while (fen[fen_idx] != ' ') {
    if (fen[fen_idx] == 'K') {
      castling_rights[Colour::White][CastlingType::Kingside] = true;
    } else if (fen[fen_idx] == 'Q') {
      castling_rights[Colour::White][CastlingType::Queenside] = true;
    } else if (fen[fen_idx] == 'k') {
      castling_rights[Colour::Black][CastlingType::Kingside] = true;
    } else if (fen[fen_idx] == 'q') {
      castling_rights[Colour::Black][CastlingType::Queenside] = true;
    }
    fen_idx++;
  }
  fen_idx++;

  // parse en passant 
  if (fen[fen_idx] != '-') {
    int rank = fen[fen_idx + 1] - '1';
    int file = fen[fen_idx] - 'a';
    enpassant.setBit(rank, file);
    fen_idx+= 3;
  } else {
    fen_idx+=2;
  }

  // if game hasn't yet started there are no counts to parse
  if (fen_idx >= fen.size() - 2) {
    return;
  }

  // parse halfmove clock
  std::string hmc;
  while (fen[fen_idx] != ' ') {
    hmc += fen[fen_idx];
    fen_idx++;
  }
  halfmove_clock = std::stoi(hmc);
  fen_idx++;

  // parse fullmove count
  std::string fmc;
  while (fen_idx < fen.size() && fen[fen_idx] != ' ') {
    fmc += fen[fen_idx];
    fen_idx++;
  }
  fullmove_cnt = std::stoi(fmc);
}

Position::Position(const std::string& fen) {
  parseFEN(fen);
}

void Position::print() const {
  // print metadata
  printf("K: %d Q: %d k: %d q: %d hmc: %d fmc: %d\n",
         castling_rights[Colour::White][CastlingType::Kingside],
         castling_rights[Colour::White][CastlingType::Queenside],
         castling_rights[Colour::Black][CastlingType::Kingside],
         castling_rights[Colour::Black][CastlingType::Queenside],
         halfmove_clock, fullmove_cnt);
  // print board state
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

BitBoard Position::getPieceBitBoard(Colour colour, PieceType piece_type) const {
  return bit_boards[colour][piece_type];
}

std::array<BitBoard, 7> Position::getPieceBitBoardsByColour(Colour colour) const {
  return bit_boards[colour];
}

BitBoard Position::getAllPiecesBitBoard() const {
  return all_pieces;
}

BitBoard Position::getEnpassantBitBoard() const {
  return enpassant;
}

Colour Position::getSideToMove() const {
  return side_to_move;
}

bool Position::canCastle(Colour colour, CastlingType castling_type) const {
  return castling_rights[colour][castling_type];
}

bool Position::isDrawBy50Moves() const {
  return halfmove_clock >= MAX_HALFMOVE_CNT;
}

bool Position::isDrawByInsufficientMaterial() const {
  std::array<BitBoard, 7> w = getPieceBitBoardsByColour(Colour::White);
  std::array<BitBoard, 7> b = getPieceBitBoardsByColour(Colour::Black);

  // if any queens, rooks or pawns are still on the board no draw is possible
  bool qrp = !w[PieceType::Queen].isEmpty() || !w[PieceType::Rook].isEmpty() ||
             !w[PieceType::Pawn].isEmpty() ||
             !b[PieceType::Queen].isEmpty() ||
             !b[PieceType::Rook].isEmpty() ||
             !b[PieceType::Pawn].isEmpty();
  // check each of the possible draw combos
  if (!qrp) {
    // king v king
    bool kk = w[PieceType::Bishop].isEmpty() && w[PieceType::Knight].isEmpty() &&
              b[PieceType::Bishop].isEmpty() && b[PieceType::Knight].isEmpty();
    
    // king and bishop vs king
    bool kbk = w[PieceType::Bishop].countSetBits() == 1 &&
               w[PieceType::Knight].isEmpty() && b[PieceType::Bishop].isEmpty() &&
               b[PieceType::Knight].isEmpty();
    
    // king and knight vs king
    bool knk = w[PieceType::Knight].countSetBits() == 1 &&
               w[PieceType::Bishop].isEmpty() && b[PieceType::Bishop].isEmpty() &&
               b[PieceType::Knight].isEmpty();

    // king vs king and bishop
    bool kkb = w[PieceType::Bishop].isEmpty() && w[PieceType::Knight].isEmpty() &&
               b[PieceType::Bishop].countSetBits() == 1 &&
               b[PieceType::Knight].isEmpty();

    // king vs king and knight
    bool kkn = w[PieceType::Bishop].isEmpty() && w[PieceType::Knight].isEmpty() &&
               b[PieceType::Bishop].isEmpty() &&
               b[PieceType::Knight].countSetBits() == 1;

    // king and bishop vs king and bishop. Only a draw if bishops are opposite
    // coloured!
    bool kbkb = w[PieceType::Bishop].countSetBits() == 1 &&
                w[PieceType::Knight].isEmpty() &&
                b[PieceType::Bishop].countSetBits() == 1 &&
                w[PieceType::Knight].isEmpty();
    bool same_colour = isWhiteSquare(w[PieceType::Bishop].getLowestSetBit()) ==
                       isWhiteSquare(b[PieceType::Bishop].getLowestSetBit());
    
    // if any of the above scenarios are true then we have a draw
    if (kk || kbk || knk || kkb || kkn || (kbkb && same_colour)) {
      return true;
    }
  } 

  return false;
}
#include "position.h"
#include "constants.h"
#include "utils.h"
#include <cstdio>

void Move::print(const Position& pos, bool minimal) const {
 printf("%s\n", to_string(pos, minimal).c_str());
}

std::string Move::to_string(const Position& pos, bool minimal) const {
  std::pair<Colour, PieceType> colour_piece = pos.getColourPieceType(source);
  std::string piece_name = piece_to_string[colour_piece.first][colour_piece.second].c_str();
  std::string source_name = indexToName(source);
  std::string dest_name = indexToName(dest);
  if (minimal) {
    return source_name + dest_name;
  }
  return piece_name + " " + source_name + " " + dest_name;
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
      addPiece(piece_info.first, piece_info.second, rankFileToIndex(rank, file));
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

void Position::addPiece(Colour colour, PieceType piece_type, int square_bit_index) {
  bit_boards[colour][piece_type].setBit(square_bit_index);
  bit_boards[colour][PieceType::All].setBit(square_bit_index);
  all_pieces.setBit(square_bit_index);
}

void Position::removePiece(Colour colour, PieceType piece_type, int square_bit_index) {
  bit_boards[colour][piece_type].clearBit(square_bit_index);
  bit_boards[colour][PieceType::All].clearBit(square_bit_index);
  all_pieces.clearBit(square_bit_index);
}

std::pair<Colour, PieceType> Position::getColourPieceType(int square_bit_index) const {
  for (int colour = Colour::White; colour <= Colour::Black; colour++) {
    for (int piece = PieceType::Pawn; piece < PieceType::All; piece++) {
      if (bit_boards[colour][piece].getBit(square_bit_index)) {
        return std::pair<Colour, PieceType>(static_cast<Colour>(colour),
                                            static_cast<PieceType>(piece));
      }
    }
  }
}

PieceType Position::getPieceType(Colour colour, int square_bit_index) const {
  for (int piece = PieceType::Pawn; piece < PieceType::All; piece++) {
    if (bit_boards[colour][piece].getBit(square_bit_index)) {
      return static_cast<PieceType>(piece);
    }
  }
  return PieceType::None;
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



void Position::makeMove(const Move& move) {
  PieceType piece_type = getPieceType(side_to_move, move.source);
  // if the king moves we lose castling rights
  if (piece_type == PieceType::King) {
    castling_rights[side_to_move] = {false, false};
  }
  // if castle moves from init position we lose castling rights on that side
  if (piece_type == PieceType::Rook) {
    prepareRookMove(move);
  }
  
  // clear last turn's enpassant board
  // TODO: check this doesn't break any of makeMove()
  enpassant.clear();
  if (piece_type == PieceType::Pawn) {
    int source_rank = indexToRank(move.source);
    int dest_rank = indexToRank(move.dest);
    // if a double pawn push enpassant is possible
    if (abs(source_rank - dest_rank) == 2) {
      prepareDoublePawnPush(move);
    }
  }

  if (move.move_type ==  MoveType::Quiet && move.promotion == PieceType::All) {
    removePiece(side_to_move, piece_type, move.source);
    addPiece(side_to_move, piece_type, move.dest);
  } else if (move.move_type == MoveType::Capture && move.promotion == PieceType::All) {
    makeCapture(move);
    removePiece(side_to_move, piece_type, move.source);
    addPiece(side_to_move, piece_type, move.dest);
  } else if (move.move_type == MoveType::EnPassantCapture) {
    int enpassant_offset = (side_to_move == Colour::White) ? -8: 8;  
    removePiece(invertColour(side_to_move), PieceType::Pawn, move.dest + enpassant_offset);
    removePiece(side_to_move, piece_type, move.source);
    addPiece(side_to_move, piece_type, move.dest);
  } else if (move.move_type == MoveType::Quiet && move.promotion != PieceType::All) {
    removePiece(side_to_move, piece_type, move.source);
    addPiece(side_to_move, move.promotion, move.dest);
  } else if (move.move_type == MoveType::Capture && move.promotion != PieceType::All) {
    makeCapture(move);
    removePiece(side_to_move, piece_type, move.source);
    addPiece(side_to_move, move.promotion, move.dest);
  } else if (move.move_type == MoveType::KingsideCastle || move.move_type == MoveType::QueensideCastle) {
    makeCastle(move);
  }
  side_to_move = invertColour(side_to_move);
  // capturing or moving a pawn resets the halfmove clock
  if (move.move_type == MoveType::Capture || move.move_type == MoveType::EnPassantCapture || piece_type == PieceType::Pawn) {
    halfmove_clock = 0;
  } else {
    halfmove_clock++;
  }
}

Position Position::applyMove(const Move& move) const {
  Position new_pos(*this);
  new_pos.makeMove(move);
  return new_pos;
}

void Position::makeCapture(const Move& move) {
  PieceType enemy_piece_type = getPieceType(invertColour(side_to_move), move.dest); 
  // capturing a rook can remove castling rights
  if (enemy_piece_type == PieceType::Rook) {
    if (side_to_move == Colour::White) {
      if (move.dest == BLACK_KINGSIDE_ROOK_INIT_INDEX) {
        castling_rights[Colour::Black][CastlingType::Kingside] = false;
      } else if (move.dest == BLACK_QUEENSIDE_ROOK_INIT_INDEX) {
        castling_rights[Colour::Black][CastlingType::Queenside] = false;
      }
    } else {
      if (move.dest == WHITE_KINGSIDE_ROOK_INIT_INDEX) {
        castling_rights[Colour::White][CastlingType::Kingside] = false;
      } else if (move.dest == WHITE_QUEENSIDE_ROOK_INIT_INDEX) {
        castling_rights[Colour::White][CastlingType::Queenside] = false;
      }
    }
  }
  removePiece(invertColour(side_to_move), enemy_piece_type, move.dest);
}

void Position::makeCastle(const Move& move) {
  if (side_to_move == Colour::White) {
    if (move.move_type == MoveType::KingsideCastle) {
      removePiece(side_to_move, PieceType::Rook, WHITE_KINGSIDE_ROOK_INIT_INDEX);
      removePiece(side_to_move, PieceType::King, 4);
      addPiece(side_to_move, PieceType::Rook, 5);
      addPiece(side_to_move, PieceType::King, 6);
    } else if (move.move_type == MoveType::QueensideCastle) {
      removePiece(side_to_move, PieceType::Rook, WHITE_QUEENSIDE_ROOK_INIT_INDEX);
      removePiece(side_to_move, PieceType::King, 4);
      addPiece(side_to_move, PieceType::Rook, 3);
      addPiece(side_to_move, PieceType::King, 2);
    }
  } else {
    if (move.move_type == MoveType::KingsideCastle) {
      removePiece(side_to_move, PieceType::Rook, BLACK_KINGSIDE_ROOK_INIT_INDEX);
      removePiece(side_to_move, PieceType::King, 60);
      addPiece(side_to_move, PieceType::Rook, 61);
      addPiece(side_to_move, PieceType::King, 62);
    } else if (move.move_type == MoveType::QueensideCastle) {
      removePiece(side_to_move, PieceType::Rook, BLACK_QUEENSIDE_ROOK_INIT_INDEX);
      removePiece(side_to_move, PieceType::King, 60);
      addPiece(side_to_move, PieceType::Rook, 59);
      addPiece(side_to_move, PieceType::King, 58);
    }
  }
}

void Position::prepareDoublePawnPush(const Move& move) {
  if (side_to_move == Colour::White) {
    enpassant.setBit(move.dest - 8);
  } else {
    enpassant.setBit(move.dest + 8);
  }
}

void Position::prepareRookMove(const Move& move) {
  if (side_to_move == Colour::White) {
    if (move.source == WHITE_KINGSIDE_ROOK_INIT_INDEX) {
      castling_rights[side_to_move][CastlingType::Kingside] = false;
    } else if (move.source == WHITE_QUEENSIDE_ROOK_INIT_INDEX) {
      castling_rights[side_to_move][CastlingType::Queenside] = false;
    }
  } else {
    if (move.source == BLACK_KINGSIDE_ROOK_INIT_INDEX) {
      castling_rights[side_to_move][CastlingType::Kingside] = false;
    } else if (move.source == BLACK_QUEENSIDE_ROOK_INIT_INDEX) {
      castling_rights[side_to_move][CastlingType::Queenside] = false;
    }
  }
}
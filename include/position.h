#ifndef POSITION_H 
#define POSITION_H 

#include <array>
#include <ostream>
#include <unordered_map>

#include "bitboard.h"
#include "constants.h"
#include "useful_fens.h"
#include "zobrist_hash.h"



class Position;

// TODO: decide what on earth this class will look like
class Move {
  public:
    // promotion=PieceType::All means no promotion
    Move(int source, int dest, MoveType move_type, PieceType promotion = PieceType::All) : source(source), dest(dest), move_type(move_type), promotion(promotion) {};
    // prints a more human readable move description
    void print(const Position& pos, const bool minimal=false) const;
    std::string to_string(const Position& pos, const bool minimal) const;

    int source;
    int dest;
    MoveType move_type;
    PieceType promotion;
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

    void addPiece(Colour colour, PieceType piece_type, int square_bit_index);
    void removePiece(Colour colour, PieceType piece_type, int square_bit_index);
    // returns piece colour and type on given square
    std::pair<Colour, PieceType> getColourPieceType(int square_bit_index) const;
    PieceType getPieceType(Colour colour, int square_bit_index) const;

    // methods needed for detecting draws
    bool isDrawBy50Moves() const;
    bool isDrawByInsufficientMaterial() const;
    bool isDrawByRepetition() const;

    // makes the given move in place
    void makeMove(const Move& move);
    // returns a new position with the given move made
    Position applyMove(const Move& move) const;

  private:
    void prepareDoublePawnPush(const Move& move);
    void prepareRookMove(const Move& move);
    void makeCapture(const Move& move);
    void makeRookCapture(const Move& move);
    void makeCastle(const Move& move);

    // NOTE: is this double array + member more horrible than 1 enum with all
    // options?
    std::array<std::array<BitBoard, 7>, 2> bit_boards; 
    BitBoard all_pieces;
    // bitboard of squares that taking pawns would move to if en-passanting i.e
    // the single pawn push square
    BitBoard enpassant;
    Colour side_to_move;
    std::array<std::array<bool, 2>, 2> castling_rights;
    int halfmove_clock = 0;
    int fullmove_cnt = 0;
    ZobristHash hash;
    std::unordered_map<unsigned long long, unsigned> hash_cnt;
};

#endif // POSITION_H 
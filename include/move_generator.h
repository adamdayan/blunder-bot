#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <array>
#include <vector>

#include "position.h"
#include "utils.h"
#include "constants.h"

enum MoveType {
  Quiet,
  Capture,
  EnPassantCapture,
  KingsideCastle,
  QueensideCastle,
  // TODO: add promotion?
};

// TODO: decide what on earth this class will look like
class Move {
  public:
    // promotion=PieceType::All means no promotion
    Move(int source, int dest, MoveType move_type, PieceType promotion = PieceType::All) : source(source), dest(dest), move_type(move_type), promotion(promotion) {};

    int source;
    int dest;
    MoveType move_type;
    PieceType promotion;
};

class BoardPerspective {
  public:
    BoardPerspective(Colour side_to_move) : side_to_move(side_to_move) {
      if (side_to_move == Colour::White) {
        opponent = Colour::Black;
        pawn_start_rank = RANK2;  
        pawn_promote_rank = RANK8;
        pawn_pre_promote_rank = RANK7;
        double_push_possible = RANK3;
        up = Direction::North;
        up_east = Direction::NorthEast;
        down_east = Direction::SouthEast;
        down = Direction::South;
        down_west = Direction::SouthWest;
        up_west = Direction::NorthWest;
        offset_sign = 1;
      } else {
        opponent = Colour::White;
        pawn_start_rank = RANK7;
        pawn_promote_rank = RANK1;
        pawn_pre_promote_rank = RANK2;
        double_push_possible = RANK6;
        up = Direction::South; 
        up = Direction::South;
        up_east = Direction::SouthEast;
        down_east = Direction::NorthEast;
        down = Direction::North;
        down_west = Direction::NorthWest;
        up_west = Direction::SouthWest;
        offset_sign = -1;
      }
    }
    Colour side_to_move;
    Colour opponent;
    BoardBits pawn_start_rank;
    BoardBits pawn_promote_rank;
    BoardBits pawn_pre_promote_rank;
    BoardBits double_push_possible;
    Direction up;
    Direction up_east;
    Direction down_east; 
    Direction down;
    Direction down_west;
    Direction up_west;
    int offset_sign;
};


using MoveVec = std::vector<Move>;

class MoveGenerator {
  public:
    // do all precomputation here
    MoveGenerator();
    // computes all legal moves
    MoveVec generateMoves(const Position& pos);
    // all the below generateX methods generate pseudo-legal moves only. We
    // prune the illegal moves in a post-processing step
    void generatePawnMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateQuietPawnPushes(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generatePawnCaptures(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generatePromotions(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateEnPassant(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateKnightMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateBishopMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateRookMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateQueenMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateKingMoves(const Position& pos, const BoardPerspective& persp, MoveVec& moves);
    void generateCastles(const Position& pos, const BoardPerspective& persp, MoveVec& moves);

    bool isLegal(const Move& move, const Position& pos, BoardPerspective& persp, int king_index, BitBoard checkers, BitBoard blockers);
    bool isLegalKingMove(const Move& move, const Position& pos, const BoardPerspective& persp);
    bool isLegalNonKingMove(const Move& move, const Position& pos, const BoardPerspective& persp);

    BitBoard computeSinglePawnPushes(const Position& pos, const BoardPerspective& persp);
    BitBoard computeDoublePawnPushes(const Position& pos, const BoardPerspective& persp, BitBoard single_pushes);
    BitBoard computeEnPassant(const Position& pos, const BoardPerspective& persp);

    BitBoard computeWestPawnCaptures(const Position& pos, const BoardPerspective& persp, const BitBoard& relevant_pawns, const BitBoard& takeable_pieces);
    BitBoard computeEastPawnCaptures(const Position& pos, const BoardPerspective& persp, const BitBoard& relevant_pawns, const BitBoard& takeable_pieces);

    // returns bitboard of all moves for a given rook
    BitBoard computeRookMoves(const Position& pos, const BoardPerspective& persp, int rook_index);
    // returns bitboard of all moves in a given direction for a given rook
    BitBoard computeRookRayMoves(const Position& pos, const BoardPerspective& persp, BitBoard ray, const Direction dir, int rook_index);
    // returns bitboard of all moves for a given bishop
    BitBoard computeBishopMoves(const Position& pos, const BoardPerspective& persp, int bishop_index);
    // returns bitboard of all moves in a given direction for a given bishop
    BitBoard computeBishopRayMoves(const Position& pos, const BoardPerspective& persp, BitBoard ray, const Direction dir, int bishop_index);
    // returns bitboard of all moves for a given queen 
    BitBoard computeQueenMoves(const Position&  pos, const BoardPerspective& persp, int queen_index);

    // returns bitboard of all attackers of the given square
    BitBoard getAttackers(const Position& pos, const BoardPerspective& persp, int square_bit_index);

  private:
    void extractPawnMoves(BitBoard bb, int offset, MoveType type, MoveVec& moves, PieceType promotion = PieceType::All);
    void extractPieceMoves(BitBoard bb, int source, MoveType type, MoveVec& moves);
    BoardMatrix<std::array<BitBoard, 8>> rays;
    BoardMatrix<BitBoard> knight_moves;
    BoardMatrix<BitBoard> king_moves;
};

#endif // MOVE_GENERATOR_H
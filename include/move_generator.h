#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <array>
#include <vector>

#include "position.h"
#include "utils.h"
#include "constants.h"

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

    // returns true if the given move is legal. used to prune pseudo-legal moves
    bool isLegal(const Move& move, const Position& pos, BoardPerspective& persp, int king_index, const BitBoard& checkers, const BitBoard& pinned_pieces);
    bool isLegalKingMove(const Move& move, const Position& pos, const BoardPerspective& persp, const BitBoard& checkers);
    bool isLegalNonKingMove(const Move& move, const Position& pos, const BoardPerspective& persp, int king_index, const BitBoard& checkers, const BitBoard& pinned_pieces);
    bool isLegalCastles(const Move& move, const Position& pos, const BoardPerspective& persp, const BitBoard& checkers);
    bool isLegalEnpassant(const Move& move, const Position& pos, const BoardPerspective& persp, int king_index, const BitBoard& checkers, const BitBoard& pinned_pieces);
    bool isLegalPinnedMove(const Move& move, const Position&  pos, const BoardPerspective& persp, int king_index);

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

    // returns a bitboard of all absolutely pinned pieces
    BitBoard getPinnedPieces(const Position& pos, const BoardPerspective& persp);

    // returns total number of legal moves from the given position to the given depth
    int computePerft(const Position& pos, int depth);
    // prints the moves 1-deep from the given position and the count of legal
    // child moves from those 1-deep positions
    void dividePerft(const Position& pos, int depth);

  private:
    void extractPawnMoves(BitBoard bb, int offset, MoveType type, MoveVec& moves, PieceType promotion = PieceType::None);
    void extractPieceMoves(BitBoard bb, int source, MoveType type, MoveVec& moves);
    BitBoard getRayBetween(int s1_index, int s2_index);
    BoardMatrix<std::array<BitBoard, 8>> rays;
    BoardMatrix<BitBoard> knight_moves;
    BoardMatrix<BitBoard> king_moves;
    BoardMatrix<BoardMatrix<BitBoard>> rays_between;
};

#endif // MOVE_GENERATOR_H
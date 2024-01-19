#ifndef USEFUL_FENS_H
#define USEFUL_FENS_H

#include <string>

// some useful FEN-encoded positions
const std::string empty_board = "8/8/8/8/8/8/8/8 w - - ";
// NOTE: is fullmove count = 1 right in the start_position?
const std::string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
const std::string tricky_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
const std::string enpassant_position = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq e4 0 1 ";

#endif // USEFUL_FENS_H
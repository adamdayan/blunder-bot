#ifndef UTILS_H
#define UTILS_H

#include <array> 
#include <string>
#include <vector>

#include "constants.h"

// convenience template alias for board-like (8x8) matrix
template<typename T> using BoardMatrix = std::array<std::array<T, 8>, 8>;

int indexToRank(int index);

int indexToFile(int index);

int rankFileToIndex(int rank, int file);

std::string indexToName(int index);

std::string moveTypeToName(MoveType type);

bool isWhiteSquare(int rank, int file);

bool isWhiteSquare(int index);

Colour invertColour(Colour colour);

# endif // UTILS_H
#ifndef UTILS_H
#define UTILS_H

#include <array> 

// convenience template alias for board-like (8x8) matrix
template<typename T> using BoardMatrix = std::array<std::array<T, 8>, 8>;

int indexToRank(int index);

int indexToFile(int index);

int rankFileToIndex(int rank, int file);

bool isWhiteSquare(int rank, int file);

bool isWhiteSquare(int index);


# endif // UTILS_H
#ifndef UTILS_H
#define UTILS_H

#include <array> 

// convenience template alias for board-like (8x8) matrix
template<typename T> using BoardMatrix = std::array<std::array<T, 8>, 8>;

int indexToRank(int index) {
  return index / 8;
}

int indexToFile(int index) {
  return index % 8;
}

int rankFileToIndex(int rank, int file) {
  return (rank * 8) + file;
}


# endif // UTILS_H
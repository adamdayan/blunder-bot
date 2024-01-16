#include "utils.h"

int indexToRank(int index) {
  return index / 8;
}

int indexToFile(int index) {
  return index % 8;
}

int rankFileToIndex(int rank, int file) {
  return (rank * 8) + file;
}

bool isWhiteSquare(int rank, int file) {
  if (rank % 2 == 0) {
    return file % 2 != 0;
  }
  return file % 2 == 0;
}

bool isWhiteSquare(int index) {
  int rank = indexToRank(index);
  int file = indexToFile(index);
  return isWhiteSquare(rank, file);
}

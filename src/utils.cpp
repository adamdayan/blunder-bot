#include "utils.h"
#include "constants.h"

int indexToRank(int index) {
  return index / 8;
}

int indexToFile(int index) {
  return index % 8;
}

int rankFileToIndex(int rank, int file) {
  return (rank * 8) + file;
}

std::string indexToName(int index) {
  return file_names[indexToFile(index)] + std::to_string(indexToRank(index) + 1);
}

std::string moveTypeToName(MoveType type) {
  if (type == MoveType::Quiet) {
    return "Quiet";
  } else if (type == MoveType::Capture) {
    return "Capture";
  } else if (type == MoveType::EnPassantCapture) {
    return "EnPassant";
  } else if (type == MoveType::KingsideCastle) {
    return "KingsideCastle";
  } else if (type == MoveType::QueensideCastle) {
    return "QueensideCastle";
  }
  return "Unknown";
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

Colour invertColour(Colour colour) {
  if (colour == Colour::White) {
    return Colour::Black;
  } else {
    return Colour::White;
  }
}

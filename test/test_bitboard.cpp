#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "position.h"

TEST_CASE("bitboard is initialised empty", "[bitboard]") {
  BitBoard bb;
  REQUIRE( bb.board == 0 );
}

TEST_CASE("initialising bitboard with value works", "[bitboard]") {
  BitBoard bb(8);
  REQUIRE( bb.board == 8 );
}

TEST_CASE("test bit_index getBit", "[bitboard]") {
  BitBoard bb(8);
  for (int i = 0; i < 64; i++) {
    if (i == 3) {
      REQUIRE(bb.getBit(i));
    } else {
      REQUIRE_FALSE(bb.getBit(i));
    }
  }
}

TEST_CASE("test rank / file getBit", "[bitboard]") {
  BitBoard bb(256);
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      if (rank == 1 && file == 0) {
        REQUIRE(bb.getBit(rank, file));
      } else {
        REQUIRE_FALSE(bb.getBit(rank, file));
      }
    }
  }
}

TEST_CASE("test empty on empty board", "[bitboard]") {
  BitBoard bb;
  REQUIRE(bb.empty());
} 

TEST_CASE("test empty() on non empty board", "[bitboard]") {
  BitBoard bb(34);
  REQUIRE_FALSE(bb.empty());
}

TEST_CASE("test bit_index setBit", "[bitboard]") {
  BitBoard bb;
  bb.setBit(16);
  // 2**16 because 0 indexed
  REQUIRE(bb.board == std::pow(2, 16));
}

TEST_CASE("test rank / file setBit", "[bitboard]") {
  BitBoard bb;
  bb.setBit(2, 1);
  REQUIRE(bb.board == std::pow(2, 17));
}

TEST_CASE("test getLowestSetBit with no set bits", "[bitboard]") {
  BitBoard bb;
  REQUIRE(bb.getLowestSetBit() == -1);
}

TEST_CASE("test getHighestSetBit with no set bits", "[bitboard]") {
  BitBoard bb;
  REQUIRE(bb.getHighestSetBit() == -1);
}

TEST_CASE("test popLowestSetBit with no set bits", "[bitboard]") {
  BitBoard bb;
  REQUIRE(bb.popLowestSetBit() == -1);
}

TEST_CASE("test popHighestSetBit with no set bits", "[bitboard]") {
  BitBoard bb;
  REQUIRE(bb.popHighestSetBit() == -1);
}

TEST_CASE("test getLowestSetBit with high-low set bits", "[bitboard]") {
  BitBoard bb;
  bb.setBit(0);
  bb.setBit(63);
  REQUIRE(bb.getLowestSetBit() == 0);
}

TEST_CASE("test getHighestSetBit with high-low set bits", "[bitboard]") {
  BitBoard bb;
  bb.setBit(0);
  bb.setBit(63);
  REQUIRE(bb.getHighestSetBit() == 63);
}

TEST_CASE("test popLowestSetBit with high-low set bits", "[bitboard]") {
  BitBoard bb;
  bb.setBit(0);
  bb.setBit(63);
  REQUIRE(bb.popLowestSetBit() == 0);
  REQUIRE_FALSE(bb.getBit(0));
  REQUIRE(bb.getBit(63));
}

TEST_CASE("test popHighestSetBit with high-low set bits", "[bitboard]") {
  BitBoard bb;
  bb.setBit(0);
  bb.setBit(63);
  REQUIRE(bb.popHighestSetBit() == 63);
  REQUIRE_FALSE(bb.getBit(63));
  REQUIRE(bb.getBit(0));
}

TEST_CASE("test clearBit", "[bitboard]") {
  BitBoard bb;
  bb.setBit(50);
  REQUIRE(bb.getBit(50));
  bb.clearBit(50);
  REQUIRE_FALSE(bb.getBit(50));
}

TEST_CASE("test clear", "[bitboard]") {
  BitBoard bb(100);
  bb.clear();
  REQUIRE(bb.board == 0);
}

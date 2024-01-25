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
  REQUIRE(bb.isEmpty());
} 

TEST_CASE("test empty() on non empty board", "[bitboard]") {
  BitBoard bb(34);
  REQUIRE_FALSE(bb.isEmpty());
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

TEST_CASE("test shift", "[bitboard]") {
  BitBoard bb(1);

  BitBoard north_shifted = bb.shift(Direction::North);
  REQUIRE(north_shifted.board == 256);

  BitBoard north_east_shifted = bb.shift(Direction::NorthEast);
  REQUIRE(north_east_shifted.board == 512);

  BitBoard east_shifted = bb.shift(Direction::East);
  REQUIRE(east_shifted.board == 2);

  BitBoard bad_west_shifted = bb.shift(Direction::West);
  REQUIRE(bad_west_shifted.board == 0);

  BitBoard bad_north_west_shifted = bb.shift(Direction::NorthWest);
  REQUIRE(bad_north_west_shifted.board == 0);

  BitBoard bb2(512);
  BitBoard south_east_shifted = bb2.shift(Direction::SouthEast);
  REQUIRE(south_east_shifted.board == 4);

  BitBoard south_shifted = bb2.shift(Direction::South);
  REQUIRE(south_shifted.board == 2);

  BitBoard south_west_shifted = bb2.shift(Direction::SouthWest);
  REQUIRE(south_west_shifted.board == 1); 

  BitBoard west_shifted = bb2.shift(Direction::West);
  REQUIRE(west_shifted.board == 256);

  BitBoard north_west_shifted = bb2.shift(Direction::NorthWest);
  REQUIRE(north_west_shifted.board == 65536);

  BitBoard bb3(128);
  BitBoard  bad_east_shifted = bb3.shift(Direction::East);
  REQUIRE(bad_east_shifted.board == 0);

  BitBoard  bad_north_east_shifted = bb3.shift(Direction::NorthEast);
  REQUIRE(bad_north_east_shifted.board == 0);
}

TEST_CASE("test clearBitsAbove", "[bitboard]") {
  BitBoard bb(15);
  bb.clearBitsAbove(2);
  REQUIRE(bb.board == 3);
}

TEST_CASE("test clearBitsBelow", "[bitboard]") {
  BitBoard bb(15);
  bb.clearBitsBelow(2);
  REQUIRE(bb.board == 8);
}



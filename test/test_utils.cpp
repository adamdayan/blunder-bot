#include <catch2/catch_test_macros.hpp>

#include "utils.h"

TEST_CASE("test indexToRank()", "[utils]") {
  REQUIRE(indexToRank(3) == 0);
  REQUIRE(indexToRank(63) == 7);
}

TEST_CASE("test indexToFile()", "[utils]") {
  REQUIRE(indexToFile(0) == 0);
  REQUIRE(indexToFile(7) == 7);
  REQUIRE(indexToFile(63) == 7);
}

TEST_CASE("test isWhiteSquare()", "[utils]") {
  REQUIRE_FALSE(isWhiteSquare(0, 0));
  REQUIRE_FALSE(isWhiteSquare(0));
  REQUIRE(isWhiteSquare(0, 1));
  REQUIRE(isWhiteSquare(1));
  REQUIRE_FALSE(isWhiteSquare(7, 7));
  REQUIRE_FALSE(isWhiteSquare(63));
}
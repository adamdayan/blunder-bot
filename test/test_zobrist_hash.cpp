#include <catch2/catch_test_macros.hpp>

#include "zobrist_hash.h"
#include "position.h"


TEST_CASE("test different positions have different hashes", "[zobrist_hash]") {
  ZobristHash::initialiseKeys();
  std::string pos_1 =  "8/8/8/8/4pP2/8/8/8 b - f3 0 1";
  Position pos1(pos_1);
  ZobristHash hash1 = ZobristHash(pos1);

  std::string pos_2 =  "8/8/8/8/3ppP2/8/8/8 b - f3 0 1";
  Position pos2(pos_2);
  ZobristHash hash2(pos2);

  REQUIRE(hash1.getHash() != hash2.getHash());
}

TEST_CASE("test same positions have same hash", "[zobrist_hash]") {
  ZobristHash::initialiseKeys();
  std::string pos_1 =  "8/8/8/8/4pP2/8/8/8 b - f3 0 1";
  Position pos1(pos_1);
  ZobristHash hash1 = ZobristHash(pos1);

  std::string pos_2 =  "8/8/8/8/4pP2/8/8/8 b - f3 0 1";
  Position pos2(pos_2);
  ZobristHash hash2(pos2);

  REQUIRE(hash1.getHash() == hash2.getHash());
}
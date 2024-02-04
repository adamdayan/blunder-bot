add_library(BlunderLib bitboard.cpp position.cpp utils.cpp move_generator.cpp zobrist_hash.cpp search.cpp)
target_include_directories(BlunderLib PUBLIC ../include)
add_library(BlunderLib bitboard.cpp position.cpp utils.cpp move_generator.cpp)
target_include_directories(BlunderLib PUBLIC ../include)
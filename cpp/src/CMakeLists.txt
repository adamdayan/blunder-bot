find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
message(TORCH_CXX_FLAGS="${TORCH_CXX_FLAGS}")

add_library(BlunderLib 
  bitboard.cpp position.cpp utils.cpp move_generator.cpp 
  zobrist_hash.cpp search.cpp net.cpp)
target_include_directories(BlunderLib PUBLIC ../include)
target_link_libraries(BlunderLib "${TORCH_LIBRARIES}")

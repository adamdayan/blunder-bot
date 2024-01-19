find_package(Catch2 REQUIRED)
add_executable(run_tests test_bitboard.cpp test_position.cpp test_utils.cpp test_move_generator.cpp)
target_link_libraries(run_tests Catch2::Catch2WithMain)
target_link_libraries(run_tests BlunderLib)
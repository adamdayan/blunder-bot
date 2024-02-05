cmake_minimum_required ( VERSION 3.27.0 )

project(
  blunder-bot
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH})

add_compile_options(-Wuninitialized -Wmaybe-uninitialized -Wunused-variable)
set(PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/out/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# main binary
add_subdirectory(cpp/)

add_executable(blunder_bot main.cpp)
target_link_libraries(blunder_bot BlunderLib)

# tests
add_subdirectory(test/)
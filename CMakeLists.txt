cmake_minimum_required ( VERSION 3.27.0 )

project(
  blunder-bot
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wuninitialized -Wmaybe-uninitialized -Wunused-variable)
set(PROJECT_BINARY_DIR out/)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_executable(blunder_bot main.cpp)
target_include_directories(blunder_bot PRIVATE include/)
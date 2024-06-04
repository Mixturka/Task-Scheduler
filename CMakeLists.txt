cmake_minimum_required(VERSION 3.12)

project(labwork10
  VERSION 1.0
  DESCRIPTION "C++ 2024 10"
  LANGUAGES CXX
  )

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)
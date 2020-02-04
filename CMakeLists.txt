cmake_minimum_required(VERSION 3.13)
project(JPMorgan_Chase_test)

set(CMAKE_CXX_FLAGS_DEBUG "-g3 -Wall -Wpedantic -Wextra -Wconversion -Weffc++")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_EXTENSIONS false)


include_directories(.)

add_executable(JPMorgan_Chase_test
                Super_Simple_stock.cpp
                Super_Simple_stock.hpp
                tests.cpp)

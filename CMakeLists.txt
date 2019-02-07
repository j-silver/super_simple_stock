cmake_minimum_required(VERSION 3.13)
project(JPMorgan_Chase_test)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wpedantic -Wextra -Wconversion -Weffc++")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native")
set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(JPMorgan_Chase_test
                Super_Simple_stock.cpp
                Super_Simple_stock.hpp
                tests.cpp)

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

add_library(merge_arrays merge_arrays.cpp)

add_test(test_merge merge_test.cpp)
add_executable(test_merge merge_test.cpp)
target_link_libraries(test_merge PRIVATE merge_arrays gtest gmock pthread)

add_library(tree_height tree_height.cpp)

add_test(test_tree_height test_tree_height.cpp)
add_executable(test_tree_height test_tree_height.cpp)
target_link_libraries(test_tree_height PRIVATE tree_height gtest gmock pthread)

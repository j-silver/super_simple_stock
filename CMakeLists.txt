cmake_minimum_required(VERSION 3.10)

project("Luenberger Ch13 ex14")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_EXTENSIONS false)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(CMAKE_CXX_FLAGS_DEBUG "-g3 -Wall -Wpedantic -Wextra -Wconversion -Weffc++")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

add_executable(luenberger_ch13_ex14 main.cpp luenberger_ch13ex14.cpp)

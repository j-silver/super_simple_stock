project("Luenberger Ch13 ex14")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wpedantic -Wextra -Wconversion -Weffc++")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native")

add_executable(luenberger_ch13_ex14 luenberger_ch13ex14.cpp)

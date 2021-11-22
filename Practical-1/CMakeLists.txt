cmake_minimum_required(VERSION 3.18)
project(Practical_1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Practical_1 main.c testing.c testing.h NBSet.c NBSet.h BSet.c BSet.h HashSet.c HashSet.h)

find_package(Threads REQUIRED)
target_link_libraries(Practical_1 Threads::Threads)
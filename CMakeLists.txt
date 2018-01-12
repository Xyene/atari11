cmake_minimum_required(VERSION 3.5)
project(2600)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-std=c++14")

SET(GCC_COVERAGE_COMPILE_FLAGS "-fwrapv")

add_executable(2600
        cpu.h cpu_instructions.cpp cpu.cpp addressable.h main.cpp)

cmake_minimum_required(VERSION 2.6)
project(cpparseopt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")

set(SOURCE_FILES src/cpparseopt.cpp include/cpparseopt.h examples/main.cpp)
add_definitions(-D_DEBUG)
add_executable(examples ${SOURCE_FILES})

set(SOURCE_FILES src/cpparseopt.cpp include/cpparseopt.h tests/tests.cpp)
add_executable(tests ${SOURCE_FILES})

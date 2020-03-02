# cmake version
cmake_minimum_required(VERSION 3.0)
# Project Name
project(openGLMap)

# Add c11++ flag to compiler
list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wno-narrowing")

# Add include files to search path for the shared libraries
include_directories(src)

# Build the subdirectories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(third_party/google-test)

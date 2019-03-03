cmake_minimum_required(VERSION 3.13)
project(generator)

set(CMAKE_CXX_STANDARD 14)

add_executable(generator main.cpp coordinateFrame.cpp tinyxml2.cpp)

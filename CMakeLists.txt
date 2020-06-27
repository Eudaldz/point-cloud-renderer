cmake_minimum_required(VERSION 3.5)

project(point-cloud-render LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(point-cloud-render main.cpp)

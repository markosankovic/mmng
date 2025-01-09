cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(mmng VERSION 0.0.1)

find_package(fmt CONFIG REQUIRED)

add_executable(mmng src/main.cpp)

target_link_libraries(mmng PRIVATE fmt::fmt)

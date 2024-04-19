cmake_minimum_required(VERSION 3.10)

project(1brc-cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(create_measurements src/create_measurements.cpp)

add_executable(calculate_average_baseline src/calculate_average_baseline.cpp)
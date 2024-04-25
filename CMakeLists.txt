cmake_minimum_required(VERSION 3.10)

project(1brc-cpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3") 

add_executable(calculate_average_baseline calculate_average_baseline.cpp)
add_executable(create_measurements create_measurements.cpp)
add_executable(calculate_average calculate_average.cpp)
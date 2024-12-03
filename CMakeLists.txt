cmake_minimum_required(VERSION 3.28)

project(aoc2024)

set(CMAKE_CXX_STANDARD 23)

#find_package(fmt CONFIG REQUIRED)

add_executable(day1 day1.cpp)

add_executable(day1_2 day1_2.cpp)

add_executable(day2 day2.cpp)

add_executable(day2_2 day2_2.cpp)

add_executable(day3 day3.cpp)
# add include directory
target_include_directories(day3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

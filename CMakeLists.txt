cmake_minimum_required(VERSION 3.16)
project(Ticket_System_2020ACM)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -Ofast ")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(code
        src/main.cpp)

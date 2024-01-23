cmake_minimum_required(VERSION 3.24)
project(P1 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(P1
        corrupt.c
        crc.c
        crc.h
        timer.c
        timer.h)

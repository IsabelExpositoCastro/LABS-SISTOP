cmake_minimum_required(VERSION 3.24)
project(LABS_SISTOP C)

set(CMAKE_C_STANDARD 11)

include_directories(P1)

add_executable(LABS_SISTOP
        P1/corrupt.c
        P1/crc.c
        P1/crc.h
        P1/timer.c
        P1/timer.h P1/main.c)

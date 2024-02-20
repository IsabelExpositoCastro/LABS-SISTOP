cmake_minimum_required(VERSION 3.24)
project(LABS_SISTOP C)

set(CMAKE_C_STANDARD 11)

include_directories(P1)

add_executable(LABS_SISTOP
        P3/fileManager.c
        P3/crc.c
        P3/main.c)


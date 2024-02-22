cmake_minimum_required(VERSION 3.24)
project(LABS_SISTOP C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")


include_directories(P1)

add_executable(LABS_SISTOP
        P3/fileManager.c
        P3/crc.c
        P3/main.c P1/main.c ../../../../crc.h P3/utils.c P3/myutils.c)


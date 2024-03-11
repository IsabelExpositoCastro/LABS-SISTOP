cmake_minimum_required(VERSION 3.24)
project(LABS_SISTOP C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")


include_directories(P1)

add_executable(LABS_SISTOP
        P5/main.c
        P5/crc.c
        P5/fileLock.c)

target_include_directories(LABS_SISTOP PRIVATE P1 P3)
target_link_libraries(LABS_SISTOP pthread)

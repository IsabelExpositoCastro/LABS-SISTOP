cmake_minimum_required(VERSION 3.24)
project(LABS_SISTOP C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")


include_directories(P1)

add_executable(LABS_SISTOP
        P3/fileManager.c
        P3/crc.c
        P3/myutils.c
        P3/main.c)

target_include_directories(LABS_SISTOP PRIVATE P1)
target_link_libraries(LABS_SISTOP pthread)

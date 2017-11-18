cmake_minimum_required(VERSION 3.2)
project(kmq C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -Wextra -pedantic -std=c11")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -Werror")
set(CMAKE_C_FLAGS_PACKAGE "${CMAKE_C_FLAGS_RELEASE}")

include_directories("${CMAKE_SOURCE_DIR}/include")

set(SOURCE_FILES
        src/knode.c
)

add_library(kmq SHARED ${SOURCE_FILES})
target_link_libraries(kmq event)

add_subdirectory(examples)

cmake_minimum_required(VERSION 3.2)
project(knode-cat C)
find_package(PkgConfig)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -Werror -O3")

set(SOURCES
        src/main.c
        src/options.c
        src/address-opt.c
        src/help-opt.c
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} kmq ${LIBEVETN_LIBRARY})

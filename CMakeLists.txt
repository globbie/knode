cmake_minimum_required(VERSION 3.2)
project(kmq C)
find_package(PkgConfig)

set(CMAKE_C_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11 -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -Werror")
set(CMAKE_C_FLAGS_PACKAGE "${CMAKE_C_FLAGS_RELEASE}")

pkg_check_modules(LIBEVENT REQUIRED libevent)
find_path(LIBEVENT_INCLUDE_DIR NAMES event.h PATHS ${LIBEVENT_INCLUDE_DIRS})
find_library(LIBEVENT_LIBRARY NAMES event PATHS ${LIBEVENT_LIBRARY_DIRS})
include_directories(${LIBEVENT_INCLUDE_DIR})

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glb-lib")
include_directories("${CMAKE_SOURCE_DIR}/libs/glb-lib/include")

include_directories("${CMAKE_SOURCE_DIR}/include")

set(SOURCE_FILES
        src/knode.c
        src/endpoint.c
        src/remote_endpoint.c
        src/timer.c
        src/utils/addrinfo.c
)

add_library(kmq SHARED ${SOURCE_FILES})
target_link_libraries(kmq ${LIBEVENT_LIBRARY} glb-lib)

add_subdirectory("${CMAKE_SOURCE_DIR}/examples/pub-sub-timer")

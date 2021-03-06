cmake_minimum_required(VERSION 3.2)
project(kmq C)
find_package(PkgConfig)

set(CMAKE_C_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11 -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -Werror -O3")

pkg_check_modules(LIBEVENT REQUIRED libevent)
find_path(LIBEVENT_INCLUDE_DIR NAMES event.h PATHS ${LIBEVENT_INCLUDE_DIRS})
find_library(LIBEVENT_LIBRARY NAMES event PATHS ${LIBEVENT_LIBRARY_DIRS})
include_directories(${LIBEVENT_INCLUDE_DIR})

if(NOT TARGET glb-lib)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/glb-lib")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/glb-lib/include")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
# todo: use target_include_directories

set(SOURCE_FILES
        src/knode.c
        src/endpoint.c
        src/remote_endpoint.c
        src/timer.c
        src/task.c
        src/chunk_pipeline.c
        src/utils/addrinfo.c
        src/utils/address-opt.c
)

add_library(kmq SHARED ${SOURCE_FILES})
target_link_libraries(kmq ${LIBEVENT_LIBRARY} glb-lib)
install(TARGETS kmq DESTINATION usr/lib)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tools")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples/pub-sub-timer")


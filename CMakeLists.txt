# Created by Gabriele Gaetano Fronzé on 2019-10-16.

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(rucio-fuse-posix CXX)

set (CMAKE_BUILD_TYPE DEBUG)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_C_FLAGS "-O0 -ggdb")
set (CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
set (CMAKE_C_FLAGS_RELEASE "-O0 -ggdb")

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64 -D FUSE_USE_VERSION=26")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -D FUSE_USE_VERSION=26")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall --pedantic -g")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(FUSE REQUIRED)
include_directories(${FUSE_INCLUDE_DIR})

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_subdirectory(submodules)
include_directories(include source submodules/SHA1 submodules/nlohmann_json/single_include/nlohmann)

add_library(globals
        include/globals.h
        source/globals.cpp
        )

add_library(utils
        include/utils.h
        source/utils.cpp
        )

add_library(curl-wrapper
        include/curl-REST.h
        source/curl-REST.cpp
        )
target_link_libraries(curl-wrapper utils ${CURL_LIBRARIES})


add_library(rucio-rest-api-wrapper
        include/REST-API.h
        source/REST-API.cpp
        )
target_link_libraries(rucio-rest-api-wrapper curl-wrapper utils globals)

add_library(rucio-fuse
        include/fuse-op.h
        source/fuse-op.cpp
        )
target_link_libraries(rucio-fuse utils rucio-rest-api-wrapper ${FUSE_LIBRARIES})

add_executable(rucio-fuse-main
        main.cxx
        )
target_link_libraries(rucio-fuse-main rucio-fuse globals nlohmann_json)

add_subdirectory(tests)
cmake_minimum_required(VERSION 3.14)

project(jsonparser LANGUAGES CXX)
set(PROJECT_TARGET jsonparser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file( GLOB SRCS
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.h"
    "${PROJECT_SOURCE_DIR}../../corestring/class/corestring.cpp"
    "${PROJECT_SOURCE_DIR}../../corestring/class/corestring.h"
    "${PROJECT_SOURCE_DIR}../../corestorage/corevector"
    "${PROJECT_SOURCE_DIR}../../corestorage/pairvector"
    "${PROJECT_SOURCE_DIR}../../corestorage/coremap"
)

add_executable(${PROJECT_TARGET} ${SRCS})
target_link_libraries(${PROJECT_TARGET} PUBLIC)
target_include_directories(${PROJECT_TARGET} PUBLIC)

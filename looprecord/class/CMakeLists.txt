cmake_minimum_required(VERSION 3.14)

project(looprecord LANGUAGES CXX)
set(PROJECT_TARGET looprecord)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file( GLOB SRCS
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/*.h" )

add_executable(${PROJECT_TARGET} ${SRCS})
target_link_libraries(${PROJECT_TARGET} PUBLIC)
target_include_directories(${PROJECT_TARGET} PUBLIC)

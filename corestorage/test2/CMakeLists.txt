cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(corestoragetest2 LANGUAGES CXX)
set(PROJECT_TARGET corestoragetest2)

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../looprecord/class/looprecord.h
      DESTINATION ${CMAKE_CURRENT_SOURCE_DIR} )
file( GLOB SRCS "${PROJECT_SOURCE_DIR}/*.cpp"
                "${PROJECT_SOURCE_DIR}/*.h"
                "${PROJECT_SOURCE_DIR}/../*.h")

add_executable(${PROJECT_TARGET} ${SRCS})
target_link_libraries(${PROJECT_TARGET} PUBLIC)
target_include_directories(${PROJECT_TARGET} PUBLIC)


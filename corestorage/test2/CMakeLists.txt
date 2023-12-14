cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(corestoragetest2 LANGUAGES CXX)
set(PROJECT_TARGET corestoragetest2)

file( GLOB SRCS "${PROJECT_SOURCE_DIR}/*.cpp"
                "${PROJECT_SOURCE_DIR}/../core*"
                "${PROJECT_SOURCE_DIR}/../pair*"
                "${PROJECT_SOURCE_DIR}../../looprecord/class/looprecord.h")

add_executable(${PROJECT_TARGET} ${SRCS})
target_link_libraries(${PROJECT_TARGET} PUBLIC)
target_include_directories(${PROJECT_TARGET} PUBLIC)


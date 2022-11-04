cmake_minimum_required(VERSION 3.23)
project(kvasync)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE KVASYNC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(kvasync ${KVASYNC_SRC})
target_include_directories(kvasync
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/kvasync)
add_subdirectory(test)
set(COMPILE_FLAGS "-fsanitize=thread")
set(LINK_FLAGS "-fsanitize=thread")
set_target_properties(kvasync PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
set_target_properties(kvasync PROPERTIES LINK_FLAGS ${LINK_FLAGS})
cmake_minimum_required(VERSION 3.23)
project(kvasync)
set(CMAKE_CXX_STANDARD 20)

file(GLOB KVASYNC_SRC_TEST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_subdirectory(lib/googletest)

add_executable(kvasync_test ${KVASYNC_SRC_TEST})
target_link_libraries(kvasync_test PRIVATE kvasync)
target_link_libraries(kvasync_test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

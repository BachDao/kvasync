cmake_minimum_required(VERSION 3.23)
project(kv_async)
set(CMAKE_CXX_STANDARD 20)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILE_FLAGS "-fcoroutines-ts")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILE_FLAGS "-fcoroutines")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
endif ()


file(GLOB_RECURSE KV_ASYNC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(kv_async ${KV_ASYNC_SRC})
target_include_directories(kv_async
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

##TSAN
if (BUILD_TSAN)
    set(COMPILE_FLAGS "-fsanitize=thread")
    set(LINK_FLAGS "-fsanitize=thread")
    set_target_properties(kv_async PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
    set_target_properties(kv_async PROPERTIES LINK_FLAGS ${LINK_FLAGS})
    target_link_libraries(kv_async PRIVATE tsan)
endif ()


##TEST
add_subdirectory(test)
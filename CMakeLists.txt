cmake_minimum_required(VERSION 3.5)
project(MPMCDequeExtended)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DTESTEXTENDED")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DTESTEXTENDED")

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(DIRSLST
    ConcurrentQueueBase
    ConcurrentQueueSimple
    ConcurrentQueueExtended
)

include_directories(${DIRSLST})

add_executable(tests tests.cpp)
target_link_libraries(tests ${CMAKE_THREAD_LIBS_INIT} ${GTEST_LIBRARIES} pthread)

add_executable(stringTests stringTests.cpp)
target_link_libraries(stringTests ${CMAKE_THREAD_LIBS_INIT} ${GTEST_LIBRARIES} pthread)

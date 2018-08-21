cmake_minimum_required(VERSION 3.5)
project(MPMCDequeExtended)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_package(Threads REQUIRED)

set(FILESLST
    TestCase/TestCase.cpp
    TimeMeasurement/TimeMeasurement.cpp
    main.cpp
)

set(DIRSLST
    MPMCDeque
    TestCase
    TimeMeasurement
)

include_directories(${DIRSLST})

add_executable(main ${FILESLST})
target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

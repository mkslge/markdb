cmake_minimum_required(VERSION 3.31)
project(marksql)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(testDM Tests/DmTests.cpp
        Page.cpp
        Page.h
        DiskManager.cpp
        DiskManager.h
)

add_executable(marksql main.cpp
        Page.cpp
        Page.h
        DiskManager.cpp
        DiskManager.h
)
target_include_directories(testDM PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_options(testDM PRIVATE -fsanitize=address,undefined)
target_link_options(testDM PRIVATE -fsanitize=address,undefined)

target_link_libraries(testDM GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(testDM)

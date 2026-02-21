cmake_minimum_required(VERSION 3.31)
project(marksql)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------------------------
# GoogleTest
# ---------------------------
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# ---------------------------
# Sanitizers (compile + link)
# ---------------------------
add_library(sanitizers INTERFACE)

target_compile_options(sanitizers INTERFACE
        -fsanitize=address,undefined
)

target_link_options(sanitizers INTERFACE
        -fsanitize=address,undefined
)

# ---------------------------
# DBMS IMPLEMENTATION LIBRARY
# ---------------------------
add_library(marksql_lib
        Page.cpp
        DiskManager.cpp
        BufferPoolManager/BufferPoolManager.cpp
        BufferPoolManager/Replacer/LRUReplacer.cpp

)

target_include_directories(marksql_lib PUBLIC ${CMAKE_SOURCE_DIR})
target_compile_features(marksql_lib PUBLIC cxx_std_20)
target_link_libraries(marksql_lib PUBLIC sanitizers)

# ---------------------------
# MAIN EXECUTABLE
# ---------------------------
add_executable(marksql main.cpp)
target_link_libraries(marksql PRIVATE marksql_lib)

# ---------------------------
# TESTS
# ---------------------------
add_executable(testDM Tests/DmTests.cpp)
target_link_libraries(testDM PRIVATE marksql_lib GTest::gtest_main)

add_executable(testPage Tests/PageTests.cpp)
target_link_libraries(testPage PRIVATE marksql_lib GTest::gtest_main)

add_executable(testReplacer Tests/ReplacerTests.cpp)
target_link_libraries(testReplacer PRIVATE marksql_lib GTest::gtest_main)

add_executable(testBPM Tests/BpmTests.cpp)
target_link_libraries(testBPM PRIVATE marksql_lib GTest::gtest_main)

# ---------------------------
# GTEST DISCOVERY
# ---------------------------
include(GoogleTest)
gtest_discover_tests(testDM)
gtest_discover_tests(testPage)
gtest_discover_tests(testReplacer)
gtest_discover_tests(testBPM)
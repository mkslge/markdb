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
        BinderLayer/BoundInsertStatement.cpp
        BinderLayer/BoundColumnRef.cpp
        BinderLayer/BoundTableStatement.cpp
        BinderLayer/Binder.cpp
        CatalogLayer/Column.cpp
        CatalogLayer/Schema.cpp
        CatalogLayer/TableInfo.cpp
        CatalogLayer/Catalog.cpp
        ExecutionLayer/InsertExecutor.cpp
        ExecutionLayer/MaterializedRow.cpp
        ExecutionLayer/ResultRow.cpp
        ExecutionLayer/RowCodec.cpp
        ExecutionLayer/SelectExecutor.cpp
        SyntaxLayer/ColumnExpression.cpp
        SyntaxLayer/InsertStatement.cpp
        SyntaxLayer/InsertParseResult.cpp
        SyntaxLayer/SelectStatement.cpp
        SyntaxLayer/ParseResult.cpp
        SyntaxLayer/Parser.cpp
        SyntaxLayer/Token.cpp
        SyntaxLayer/Tokenizer.cpp
        Models/Page.cpp
        DiskLayer/DiskManager.cpp
        MemoryManagementLayer/BufferPoolManager.cpp
        MemoryManagementLayer/Replacer/LRUReplacer.cpp
        RecordLayer/HeapPage.cpp
        TableLayer/TableHeap.cpp
)

target_include_directories(marksql_lib PUBLIC ${CMAKE_SOURCE_DIR})
target_compile_features(marksql_lib PUBLIC cxx_std_20)
target_link_libraries(marksql_lib PUBLIC sanitizers)

# ---------------------------
# MAIN EXECUTABLE
# ---------------------------
add_executable(marksql main.cpp

        RecordLayer/HeapPage.cpp
        Models/Tuple.cpp
        BPlusTree/BPlusTree.tpp
        TableLayer/TableHeap.cpp
)
target_link_libraries(marksql PRIVATE marksql_lib)

# ---------------------------
# TESTS
# ---------------------------
add_executable(testDM)
target_sources(testDM PRIVATE Tests/DmTests.cpp)
target_link_libraries(testDM PRIVATE marksql_lib GTest::gtest_main)
target_compile_definitions(testDM PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_executable(testPage)
target_sources(testPage PRIVATE Tests/PageTests.cpp)
target_link_libraries(testPage PRIVATE marksql_lib GTest::gtest_main)

add_executable(testReplacer)
target_sources(testReplacer PRIVATE Tests/ReplacerTests.cpp)
target_link_libraries(testReplacer PRIVATE marksql_lib GTest::gtest_main)

add_executable(testBPM)
target_sources(testBPM PRIVATE Tests/BpmTests.cpp)
target_link_libraries(testBPM PRIVATE marksql_lib GTest::gtest_main)

add_executable(testHeapPage Tests/HeapPageTests.cpp)
target_link_libraries(testHeapPage PRIVATE marksql_lib GTest::gtest_main)

add_executable(testTableHeap)
target_sources(testTableHeap PRIVATE Tests/TableHeapTests.cpp)
target_link_libraries(testTableHeap PRIVATE marksql_lib GTest::gtest_main)

add_executable(testCatalog)
target_sources(testCatalog PRIVATE Tests/CatalogTests.cpp)
target_link_libraries(testCatalog PRIVATE marksql_lib GTest::gtest_main)

add_executable(testBinder)
target_sources(testBinder PRIVATE Tests/BinderTests.cpp)
target_link_libraries(testBinder PRIVATE marksql_lib GTest::gtest_main)

add_executable(testSyntax)
target_sources(testSyntax PRIVATE Tests/SyntaxTests.cpp)
target_link_libraries(testSyntax PRIVATE marksql_lib GTest::gtest_main)

add_executable(testTokenizer)
target_sources(testTokenizer PRIVATE Tests/TokenizerTests.cpp)
target_link_libraries(testTokenizer PRIVATE marksql_lib GTest::gtest_main)

add_executable(testParser)
target_sources(testParser PRIVATE Tests/ParserTests.cpp)
target_link_libraries(testParser PRIVATE marksql_lib GTest::gtest_main)

add_executable(testExecutor)
target_sources(testExecutor PRIVATE Tests/ExecutorTests.cpp)
target_link_libraries(testExecutor PRIVATE marksql_lib GTest::gtest_main)


add_executable(testBPlusTree)
target_sources(testBPlusTree PRIVATE Tests/BPlusTreeTests.cpp BPlusTree/BPlusTree.tpp)
target_link_libraries(testBPlusTree PRIVATE marksql_lib GTest::gtest_main)

# ---------------------------
# GTEST DISCOVERY
# ---------------------------
include(GoogleTest)
gtest_discover_tests(testDM)
gtest_discover_tests(testPage)
gtest_discover_tests(testReplacer)
gtest_discover_tests(testBPM)
gtest_discover_tests(testHeapPage)
gtest_discover_tests(testTableHeap)
gtest_discover_tests(testCatalog)
gtest_discover_tests(testBinder)
gtest_discover_tests(testSyntax)
gtest_discover_tests(testTokenizer)
gtest_discover_tests(testParser)
gtest_discover_tests(testExecutor)
gtest_discover_tests(testBPlusTree)

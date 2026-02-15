//
// Created by Mark on 2/14/26.
//

#include <gtest/gtest.h>
#include "../DiskManager.h"
TEST(DmTest, DefaultConstructor) {
    DiskManager dm;
}


TEST(DmTest, WriteAndRead) {
    DiskManager dm{"/Users/markseeliger/CLionProjects/marksql/Storage/TestDBs/writeandread.db"};
    char in[PAGE_SIZE];
    memcpy(in, "Hello World", sizeof("Hello World"));


    dm.writePage(0, in);
    char* buf = static_cast<char*>(malloc(4096));
    dm.readPage(0, buf);
    std::cout << buf << std::endl;
    free(buf);
}

TEST(DmTest, AllocatePage) {
    DiskManager dm("/Users/markseeliger/CLionProjects/marksql/Storage/TestDBs/alloc.db");
    size_t firstAlloc = dm.allocatePage();
    size_t secondAlloc = dm.allocatePage();
    EXPECT_EQ(firstAlloc + 1, secondAlloc);
}


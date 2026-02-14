//
// Created by Mark on 2/14/26.
//

#include <gtest/gtest.h>
#include "../DiskManager.h"
TEST(DmTest, DefaultConstructor) {
    DiskManager dm;
}


TEST(DmTest, WriteAndRead) {
    DiskManager dm;
    char in[PAGE_SIZE];
    memcpy(in, "Hello World", sizeof("Hello World"));


    dm.writePage(0, in);
    char* buf = static_cast<char*>(malloc(4096));
    dm.readPage(0, buf);
    std::cout << buf << std::endl;
    free(buf);
}
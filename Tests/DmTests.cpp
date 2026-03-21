//
// Created by Mark on 2/14/26.
//

#include <gtest/gtest.h>
#include "../DiskLayer/DiskManager.h"

#include <array>
#include <cstring>
#include <filesystem>
#include <string>

namespace {
std::filesystem::path make_test_db_path(const std::string& filename) {
    return std::filesystem::temp_directory_path() / filename;
}
}

TEST(DmTest, WriteAndRead) {
    const std::filesystem::path db_path = make_test_db_path("marksql_writeandread.db");
    std::filesystem::remove(db_path);

    DiskManager dm(db_path.string());

    std::array<char, PAGE_SIZE> in{};
    std::memcpy(in.data(), "Hello World", sizeof("Hello World"));

    dm.writePage(0, in.data());

    std::array<char, PAGE_SIZE> out{};
    dm.readPage(0, out.data());

    EXPECT_STREQ(in.data(), out.data());

    std::filesystem::remove(db_path);
}

TEST(DmTest, AllocatePage) {
    const std::filesystem::path db_path = make_test_db_path("marksql_alloc.db");
    std::filesystem::remove(db_path);

    DiskManager dm(db_path.string());

    size_t firstAlloc = dm.allocatePage();
    size_t secondAlloc = dm.allocatePage();

    EXPECT_EQ(firstAlloc + 1, secondAlloc);

    std::filesystem::remove(db_path);
}
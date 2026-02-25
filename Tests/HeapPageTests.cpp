//
// Created by Mark on 2/25/26.
//
#include <gtest/gtest.h>
#include <cstring>
#include "../Models/HeapPage.h"


class HeapPageTest : public ::testing::Test {
protected:
    char buffer[PAGE_SIZE];

    void SetUp() override {
        std::memset(buffer, 0, PAGE_SIZE);
    }
};

TEST_F(HeapPageTest, InsertSingleTuple) {
    HeapPage page(buffer);

    Tuple tuple = {'h','e','l','l','o'};

    auto slot_id = page.insertTuple(tuple);

    ASSERT_TRUE(slot_id.has_value());
    EXPECT_EQ(page.getNumSlots(), 1);

    Tuple fetched = page.getTuple(slot_id.value());

    EXPECT_EQ(fetched, tuple);
    std::string fetched_str(fetched.begin(), fetched.end());
    EXPECT_EQ(fetched_str ,"hello");
}
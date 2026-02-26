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

TEST_F(HeapPageTest, InsertMultipleTuples) {
    HeapPage page(buffer);

    Tuple t1 = {'a','a','a'};
    Tuple t2 = {'b','b','b','b'};
    Tuple t3 = {'c'};

    auto s1 = page.insertTuple(t1);
    auto s2 = page.insertTuple(t2);
    auto s3 = page.insertTuple(t3);

    ASSERT_TRUE(s1.has_value());
    ASSERT_TRUE(s2.has_value());
    ASSERT_TRUE(s3.has_value());

    EXPECT_EQ(page.getNumSlots(), 3);

    EXPECT_EQ(page.getTuple(s1.value()), t1);
    EXPECT_EQ(page.getTuple(s2.value()), t2);
    EXPECT_EQ(page.getTuple(s3.value()), t3);
}

TEST_F(HeapPageTest, DeleteTuple) {
    HeapPage page(buffer);

    Tuple tuple = {'x','y','z'};
    auto slot_id = page.insertTuple(tuple);

    ASSERT_TRUE(slot_id.has_value());

    page.deleteTuple(slot_id.value());

    Tuple fetched = page.getTuple(slot_id.value());

    // depends on your delete semantics

}

TEST_F(HeapPageTest, InsertUntilFull) {
    HeapPage page(buffer);

    Tuple tuple(200, 'a'); // 200-byte tuple

    int count = 0;
    while (true) {
        auto result = page.insertTuple(tuple);
        if (!result.has_value()) break;
        count++;
    }

    EXPECT_GT(count, 0);

    // Ensure further insert fails
    auto fail = page.insertTuple(tuple);
    EXPECT_FALSE(fail.has_value());
}


//
// Created by Mark on 2/25/26.
//
#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include <vector>
#include "../RecordLayer/HeapPage.h"

namespace {
Tuple makeTuple(std::initializer_list<char> bytes) {
    return Tuple{std::vector<char>(bytes)};
}

Tuple makeFilledTuple(std::size_t size, char value) {
    return Tuple{std::vector<char>(size, value)};
}
}

class HeapPageTest : public ::testing::Test {
protected:
    char buffer[PAGE_SIZE];

    void SetUp() override {
        std::memset(buffer, 0, PAGE_SIZE);
    }
};

TEST_F(HeapPageTest, InsertSingleTuple) {
    HeapPage page(buffer);

    Tuple tuple = makeTuple({'h', 'e', 'l', 'l', 'o'});

    auto slot_id = page.insertTuple(tuple);

    ASSERT_TRUE(slot_id.has_value());
    EXPECT_EQ(page.getNumSlots(), 1);

    Tuple fetched = page.getTuple(slot_id.value());

    EXPECT_EQ(fetched.data_, tuple.data_);
    std::string fetched_str(fetched.data_.begin(), fetched.data_.end());
    EXPECT_EQ(fetched_str, "hello");
}

TEST_F(HeapPageTest, InsertMultipleTuples) {
    HeapPage page(buffer);

    Tuple t1 = makeTuple({'a', 'a', 'a'});
    Tuple t2 = makeTuple({'b', 'b', 'b', 'b'});
    Tuple t3 = makeTuple({'c'});

    auto s1 = page.insertTuple(t1);
    auto s2 = page.insertTuple(t2);
    auto s3 = page.insertTuple(t3);

    ASSERT_TRUE(s1.has_value());
    ASSERT_TRUE(s2.has_value());
    ASSERT_TRUE(s3.has_value());

    EXPECT_EQ(page.getNumSlots(), 3);

    EXPECT_EQ(page.getTuple(s1.value()).data_, t1.data_);
    EXPECT_EQ(page.getTuple(s2.value()).data_, t2.data_);
    EXPECT_EQ(page.getTuple(s3.value()).data_, t3.data_);
}

TEST_F(HeapPageTest, DeleteTuple) {
    HeapPage page(buffer);

    Tuple tuple = makeTuple({'x', 'y', 'z'});
    auto slot_id = page.insertTuple(tuple);

    ASSERT_TRUE(slot_id.has_value());

    page.applyDelete(slot_id.value());

    Tuple fetched = page.getTuple(slot_id.value());

    EXPECT_TRUE(fetched.data_.empty());

}

TEST_F(HeapPageTest, InsertUntilFull) {
    HeapPage page(buffer);

    Tuple tuple = makeFilledTuple(200, 'a');

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

TEST_F(HeapPageTest, SetAndGetLinkedPageIds) {
    HeapPage page(buffer);

    page.setPrevPage(11);
    page.setNextPage(22);

    EXPECT_EQ(page.getPrevPage(), 11);
    EXPECT_EQ(page.getNextPage(), 22);
}

TEST_F(HeapPageTest, ChangeTupleReplacesTupleWhenReplacementIsSameSize) {
    HeapPage page(buffer);
    Tuple original = makeTuple({'o', 'l', 'd'});
    Tuple replacement = makeTuple({'n', 'e', 'w'});

    auto slot_id = page.insertTuple(original);

    ASSERT_TRUE(slot_id.has_value());
    EXPECT_TRUE(page.changeTuple(slot_id.value(), replacement));
    EXPECT_EQ(page.getNumSlots(), 1);
    EXPECT_EQ(page.getTuple(slot_id.value()).data_, replacement.data_);
}

TEST_F(HeapPageTest, ChangeTupleCanShrinkTuple) {
    HeapPage page(buffer);
    Tuple original = makeTuple({'a', 'b', 'c', 'd', 'e', 'f'});
    Tuple replacement = makeTuple({'z', 'z'});

    auto slot_id = page.insertTuple(original);

    ASSERT_TRUE(slot_id.has_value());

    EXPECT_TRUE(page.changeTuple(slot_id.value(), replacement));
    EXPECT_EQ(page.getTuple(slot_id.value()).data_, replacement.data_);
    EXPECT_EQ(page.getNumSlots(), 1);
}

TEST_F(HeapPageTest, ChangeTupleCanGrowTuple) {
    HeapPage page(buffer);
    Tuple original = makeTuple({'o', 'l', 'd'});
    Tuple replacement = makeTuple({'n', 'e', 'w', 'e', 'r'});

    auto slot_id = page.insertTuple(original);

    ASSERT_TRUE(slot_id.has_value());

    EXPECT_TRUE(page.changeTuple(slot_id.value(), replacement));
    EXPECT_EQ(page.getTuple(slot_id.value()).data_, replacement.data_);
    EXPECT_EQ(page.getNumSlots(), 1);
}

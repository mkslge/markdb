//
// Created by Mark on 2/17/26.
//
#include <gtest/gtest.h>
#include "../Page.h"

TEST(PageTest, DefaultConstructor) {
    Page page;
    EXPECT_EQ(page.get_page_id(), INVALID_PAGE_ID);
    EXPECT_EQ(page.is_dirty(), false);
    EXPECT_EQ(page.get_pin_count(), 0);
}

TEST(PageTest, MemoryZeroInitialized) {
    Page page;
    const char* data = page.get_data();

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        EXPECT_EQ(data[i], 0);
    }
}

TEST(PageTest, SetPageId) {
    Page page;
    page.set_page_id(42);

    EXPECT_EQ(page.get_page_id(), 42);
}

TEST(PageTest, DirtyFlag) {
    Page page;

    EXPECT_FALSE(page.is_dirty());

    page.set_dirty(true);
    EXPECT_TRUE(page.is_dirty());

    page.set_dirty(false);
    EXPECT_FALSE(page.is_dirty());
}

TEST(PageTest, PinUnpin) {
    Page page;

    page.pin();
    EXPECT_EQ(page.get_pin_count(), 1);

    page.pin();
    EXPECT_EQ(page.get_pin_count(), 2);

    page.unpin();
    EXPECT_EQ(page.get_pin_count(), 1);

    page.unpin();
    EXPECT_EQ(page.get_pin_count(), 0);
}

TEST(PageTest, PinCountDoesNotGoNegative) {
    Page page;

    page.unpin();  // unpin without pinning first
    EXPECT_GE(page.get_pin_count(), 0);
}



TEST(PageTest, DataWritable) {
    Page page;

    char* data = page.get_data();
    data[0] = 123;
    data[PAGE_SIZE - 1] = 45;

    EXPECT_EQ(page.get_data()[0], 123);
    EXPECT_EQ(page.get_data()[PAGE_SIZE - 1], 45);
}

TEST(PageTest, ResetMemory) {
    Page page;

    page.get_data()[0] = 99;
    page.set_page_id(5);
    page.set_dirty(true);
    page.pin();

    page.reset();

    EXPECT_EQ(page.get_page_id(), INVALID_PAGE_ID);
    EXPECT_FALSE(page.is_dirty());
    EXPECT_EQ(page.get_pin_count(), 0);

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        EXPECT_EQ(page.get_data()[i], 0);
    }
}


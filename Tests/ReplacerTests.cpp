//
// Created by Mark on 2/19/26.
//

#include <gtest/gtest.h>
#include "../BufferPoolManager/Replacer/LRUReplacer.h"

TEST(LRUReplacerTest, VictimFromEmpty) {
    LRUReplacer r;
    int frame;

    EXPECT_FALSE(r.victim(&frame));
    EXPECT_EQ(r.size(), 0);
}


TEST(LRUReplacerTest, SingleUnpinVictim) {
    LRUReplacer r;
    int frame;

    r.unpin(7);

    EXPECT_EQ(r.size(), 1);
    EXPECT_TRUE(r.victim(&frame));
    EXPECT_EQ(frame, 7);
    EXPECT_EQ(r.size(), 0);
}


TEST(LRUReplacerTest, LRUOrder) {
    LRUReplacer r;
    int frame;

    r.unpin(1);
    r.unpin(2);
    r.unpin(3);

    EXPECT_TRUE(r.victim(&frame));
    EXPECT_EQ(frame, 1);

    EXPECT_TRUE(r.victim(&frame));
    EXPECT_EQ(frame, 2);

    EXPECT_TRUE(r.victim(&frame));
    EXPECT_EQ(frame, 3);
}

TEST(LRUReplacerTest, PinRemoves) {
    LRUReplacer r;
    int frame;

    r.unpin(1);
    r.unpin(2);

    r.pin(1);

    EXPECT_TRUE(r.victim(&frame));
    EXPECT_EQ(frame, 2);

    EXPECT_FALSE(r.victim(&frame));
}

TEST(LRUReplacerTest, NoDuplicateUnpin) {
    LRUReplacer r;

    r.unpin(5);
    r.unpin(5);
    r.unpin(5);

    EXPECT_EQ(r.size(), 1);
}

TEST(LRUReplacerTest, ReUnpinMovesToMRU) {
    LRUReplacer r;
    int frame;

    r.unpin(1);
    r.unpin(2);

    r.unpin(1);  // 1 should now be MRU

    EXPECT_TRUE(r.victim(&frame));
    EXPECT_EQ(frame, 2);  // should evict 2 first
}

TEST(LRUReplacerTest, PinThenUnpin) {
    LRUReplacer r;
    int frame;

    r.unpin(1);
    r.pin(1);
    r.unpin(1);

    EXPECT_TRUE(r.victim(&frame));
    EXPECT_EQ(frame, 1);
}
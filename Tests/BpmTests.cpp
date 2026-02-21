//
// Created by Mark on 2/21/26.
//

#include <gtest/gtest.h>
#include "../DiskManager.h"
#include "../BufferPoolManager/BufferPoolManager.h"

TEST(BufferPoolManagerTest, FetchPageFromDisk) {
    DiskManager dm("test.db");
    BufferPoolManager bpm(&dm);

    int pid = 3;

    char buf[PAGE_SIZE];
    memset(buf, 'A', PAGE_SIZE);
    dm.writePage(pid, buf);

    Page* p = bpm.fetchPage(pid);

    ASSERT_NE(p, nullptr);
    ASSERT_EQ(memcmp(p->get_data(), buf, PAGE_SIZE), 0);
}


TEST(BufferPoolManagerTest, FetchSamePageTwice) {
    DiskManager dm("test.db");
    BufferPoolManager bpm(&dm);

    Page* p1 = bpm.fetchPage(1);
    Page* p2 = bpm.fetchPage(1);

    ASSERT_EQ(p1, p2);
}


TEST(BufferPoolManagerTest, CannotEvictPinned) {
    DiskManager dm("test.db");
    BufferPoolManager bpm(&dm);

    for (int i = 0; i < 1000; i++) {
        ASSERT_NE(bpm.fetchPage(i), nullptr);
    }

    Page* p = bpm.fetchPage(1001);

    ASSERT_EQ(p, nullptr);
}


TEST(BufferPoolManagerTest, UnpinAllowsEviction) {
    DiskManager dm("test.db");
    BufferPoolManager bpm(&dm);

    for (int i = 0; i < 1000; i++) {
        bpm.fetchPage(i);
    }

    bpm.unpinPage(0, false);

    Page* p = bpm.fetchPage(1001);

    ASSERT_NE(p, nullptr);
}


TEST(BufferPoolManagerTest, DirtyWriteBack) {
    DiskManager dm("test.db");
    BufferPoolManager bpm(&dm);

    int pid = 7;

    Page* p = bpm.fetchPage(pid);
    p->get_data()[0] = 'Z';
    bpm.unpinPage(pid, true);

    for (int i = 1; i < 1000; i++) {
        bpm.fetchPage(i + 100);
        bpm.unpinPage(i + 100, false);
    }

    bpm.fetchPage(5000);

    char buf[PAGE_SIZE];
    dm.readPage(pid, buf);

    ASSERT_EQ(buf[0], 'Z');
}




TEST(BufferPoolManagerTest, LRUEvictionOrder) {
    DiskManager dm("test.db");
    BufferPoolManager bpm(&dm);

    for (int i = 0; i < 1000; i++) {
        bpm.fetchPage(i);
        bpm.unpinPage(i, false);
    }

    bpm.fetchPage(0);
    bpm.unpinPage(0, false);

    bpm.fetchPage(2000);

    Page* p1 = bpm.fetchPage(1);

    ASSERT_NE(p1, nullptr);
}


TEST(BufferPoolManagerTest, DataPersistsAfterEviction) {
    DiskManager dm("test.db");
    BufferPoolManager bpm(&dm);

    Page* p = bpm.fetchPage(5);
    p->get_data()[0] = 'X';

    bpm.unpinPage(5, true);

    bpm.fetchPage(6);
    bpm.unpinPage(6, false);

    bpm.fetchPage(7);

    Page* p2 = bpm.fetchPage(5);

    ASSERT_EQ(p2->get_data()[0], 'X');
}
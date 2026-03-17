//
// Created by Mark on 3/17/26.
//

#include <gtest/gtest.h>
#include "../DiskLayer/DiskManager.h"
#include "../MemoryManagementLayer/BufferPoolManager.h"
#include "../RecordLayer/HeapPage.h"
#include "../TableLayer/TableHeap.h"

class TableHeapTest : public ::testing::Test {
protected:
    DiskManager dm_{"/Users/markseeliger/CLionProjects/marksql/Storage/TestDBs/tableheap.db"};
    BufferPoolManager bpm_{&dm_};
};

TEST_F(TableHeapTest, ConstructorCreatesUsableTableHeap) {
    TableHeap table_heap(&dm_, &bpm_);
    
}

TEST_F(TableHeapTest, InsertTupleReturnsValidRid) {
    TableHeap table_heap(&dm_, &bpm_);
    Tuple tuple = {'h', 'e', 'l', 'l', 'o'};

    RID rid = table_heap.insertTuple(tuple);

    EXPECT_GE(rid.page_id, 0);
    EXPECT_EQ(rid.slot_id, 0);
}

TEST_F(TableHeapTest, InsertedTupleCanBeReadBackFromHeapPage) {
    TableHeap table_heap(&dm_, &bpm_);
    Tuple tuple = {'m', 'a', 'r', 'k'};

    RID rid = table_heap.insertTuple(tuple);

    Page* page = bpm_.fetchPage(rid.page_id);
    ASSERT_NE(page, nullptr);

    HeapPage heap_page(page->get_data());
    Tuple fetched = heap_page.getTuple(rid.slot_id);

    EXPECT_EQ(fetched, tuple);
    bpm_.unpinPage(rid.page_id, false);
}

TEST_F(TableHeapTest, RepeatedMixedSizeInsertsPreserveRidOrderingAndTupleBytes) {
    TableHeap table_heap(&dm_, &bpm_);

    std::vector<Tuple> tuples = {
            {'a'},
            {'b', 'b'},
            {'c', 'c', 'c'},
            {'d', 'e', 'f', 'g', 'h'},
            Tuple(32, 'x'),
            Tuple(64, 'y'),
            {'z', '\0', 'z', '\1', 'z'},
            {'m', 'a', 'r', 'k', 's', 'q', 'l'},
            Tuple(128, 'q'),
            {'l', 'a', 's', 't'}
    };

    std::vector<RID> rids;
    rids.reserve(tuples.size());

    for (const Tuple& tuple : tuples) {
        rids.push_back(table_heap.insertTuple(tuple));
    }

    ASSERT_EQ(rids.size(), tuples.size());

    const int first_page_id = rids.front().page_id;
    for (std::size_t i = 0; i < rids.size(); i++) {
        EXPECT_GE(rids[i].page_id, 0);
        EXPECT_EQ(rids[i].page_id, first_page_id);
        EXPECT_EQ(rids[i].slot_id, static_cast<int>(i));
    }

    Page* page = bpm_.fetchPage(first_page_id);
    ASSERT_NE(page, nullptr);

    HeapPage heap_page(page->get_data());
    EXPECT_EQ(heap_page.getNumSlots(), tuples.size());

    for (std::size_t i = 0; i < tuples.size(); i++) {
        Tuple fetched = heap_page.getTuple(rids[i].slot_id);
        EXPECT_EQ(fetched, tuples[i]) << "Tuple mismatch at slot " << i;
    }

    EXPECT_LT(heap_page.header()->free_space_start, heap_page.header()->free_space_end);
    bpm_.unpinPage(first_page_id, false);
}

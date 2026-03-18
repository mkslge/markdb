//
// Created by Mark on 3/17/26.
//

#include <gtest/gtest.h>
#include <vector>
#include "../DiskLayer/DiskManager.h"
#include "../MemoryManagementLayer/BufferPoolManager.h"
#include "../RecordLayer/HeapPage.h"
#include "../TableLayer/TableHeap.h"

namespace {
Tuple makeTuple(std::initializer_list<char> bytes) {
    return Tuple{std::vector<char>(bytes)};
}

Tuple makeFilledTuple(std::size_t size, char value) {
    return Tuple{std::vector<char>(size, value)};
}
}

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
    Tuple tuple = makeTuple({'h', 'e', 'l', 'l', 'o'});

    RID rid = table_heap.insertTuple(tuple);

    EXPECT_GE(rid.page_id, 0);
    EXPECT_EQ(rid.slot_id, 0);
}

TEST_F(TableHeapTest, InsertedTupleCanBeReadBackFromHeapPage) {
    TableHeap table_heap(&dm_, &bpm_);
    Tuple tuple = makeTuple({'m', 'a', 'r', 'k'});

    RID rid = table_heap.insertTuple(tuple);

    Page* page = bpm_.fetchPage(rid.page_id);
    ASSERT_NE(page, nullptr);

    HeapPage heap_page(page->get_data());
    Tuple fetched = heap_page.getTuple(rid.slot_id);

    EXPECT_EQ(fetched.data_, tuple.data_);
    bpm_.unpinPage(rid.page_id, false);
}

TEST_F(TableHeapTest, RepeatedMixedSizeInsertsPreserveRidOrderingAndTupleBytes) {
    TableHeap table_heap(&dm_, &bpm_);

    std::vector<Tuple> tuples = {
            makeTuple({'a'}),
            makeTuple({'b', 'b'}),
            makeTuple({'c', 'c', 'c'}),
            makeTuple({'d', 'e', 'f', 'g', 'h'}),
            makeFilledTuple(32, 'x'),
            makeFilledTuple(64, 'y'),
            makeTuple({'z', '\0', 'z', '\1', 'z'}),
            makeTuple({'m', 'a', 'r', 'k', 's', 'q', 'l'}),
            makeFilledTuple(128, 'q'),
            makeTuple({'l', 'a', 's', 't'})
    };

    std::vector<RID> rids;
    rids.reserve(tuples.size());

    for (Tuple& tuple : tuples) {
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
        EXPECT_EQ(fetched.data_, tuples[i].data_) << "Tuple mismatch at slot " << i;
    }

    EXPECT_LT(heap_page.header()->free_space_start, heap_page.header()->free_space_end);
    bpm_.unpinPage(first_page_id, false);
}

TEST_F(TableHeapTest, GetTupleReadsBackInsertedTupleByRid) {
    TableHeap table_heap(&dm_, &bpm_);
    Tuple inserted = makeTuple({'r', 'e', 'a', 'd'});

    RID rid = table_heap.insertTuple(inserted);
    Tuple fetched;

    ASSERT_TRUE(table_heap.getTuple(rid, fetched));
    EXPECT_EQ(fetched.data_, inserted.data_);
}

TEST_F(TableHeapTest, ApplyDeleteMakesTupleEmptyAtExistingRid) {
    TableHeap table_heap(&dm_, &bpm_);
    Tuple inserted = makeTuple({'d', 'e', 'l', 'e', 't', 'e'});

    RID rid = table_heap.insertTuple(inserted);
    ASSERT_TRUE(table_heap.applyDelete(rid));

    Tuple fetched;
    ASSERT_TRUE(table_heap.getTuple(rid, fetched));
    EXPECT_TRUE(fetched.data_.empty());
}

TEST_F(TableHeapTest, EditTupleDeletesOldRidAndReinsertsReplacementTuple) {
    TableHeap table_heap(&dm_, &bpm_);
    Tuple original = makeTuple({'o', 'l', 'd'});
    Tuple replacement = makeFilledTuple(48, 'n');

    RID original_rid = table_heap.insertTuple(original);

    ASSERT_TRUE(table_heap.editTuple(original_rid, replacement));

    Tuple old_slot_contents;
    ASSERT_TRUE(table_heap.getTuple(original_rid, old_slot_contents));
    EXPECT_TRUE(old_slot_contents.data_.empty());

    Page* page = bpm_.fetchPage(original_rid.page_id);
    ASSERT_NE(page, nullptr);

    HeapPage heap_page(page->get_data());
    EXPECT_EQ(heap_page.getNumSlots(), 2);
    EXPECT_EQ(heap_page.getTuple(1).data_, replacement.data_);
    bpm_.unpinPage(original_rid.page_id, false);
}

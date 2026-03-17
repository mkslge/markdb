//
// Created by Mark on 2/25/26.
//

#ifndef TABLEHEAP_H
#define TABLEHEAP_H

#include "../RecordLayer/HeapPage.h"
#include "MemoryManagementLayer/BufferPoolManager.h"
#include "../Models/RID.h"
class TableHeap {
private:
    DiskManager* dm_;
    BufferPoolManager* bpm_;
    int first_page_id_;
public:
    TableHeap(DiskManager* dm, BufferPoolManager* bpm);

    RID insertTuple(const Tuple& to_insert);
};



#endif //TABLEHEAP_H

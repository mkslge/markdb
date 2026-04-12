//
// Created by Mark on 2/25/26.
//

#ifndef TABLEHEAP_H
#define TABLEHEAP_H

#include "../RecordLayer/HeapPage.h"
#include "MemoryManagementLayer/BufferPoolManager.h"
#include "../Models/RID.h"
#include <vector>
class TableHeap {
private:
    DiskManager* dm_;
    BufferPoolManager* bpm_;
    int first_page_id_;
public:
    TableHeap(DiskManager* dm, BufferPoolManager* bpm);
    TableHeap(DiskManager* dm, BufferPoolManager* bpm, int first_page_id);

    RID insertTuple(Tuple& to_insert);
    bool getTuple(const RID& rid, Tuple& out);
    std::vector<Tuple> scanTuples();
    int getFirstPageId() const;

    bool applyDelete(const RID& rid);

    bool editTuple(const RID& rid, Tuple& new_tuple); //could have a tuple that we need to move the page

};



#endif //TABLEHEAP_H

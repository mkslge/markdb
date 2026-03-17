//
// Created by Mark on 2/25/26.
//

#include "TableHeap.h"


TableHeap::TableHeap(DiskManager* dm, BufferPoolManager *bpm): dm_(dm), bpm_(bpm) {
    first_page_id_ = dm_->allocatePage();
    bpm_->newPage(first_page_id_);
}


RID TableHeap::insertTuple(const Tuple& to_insert) {
    int curr_page_id = first_page_id_;
    bool inserted = false;
    RID rid{-1,-1};
    while (!inserted) {
        //grab our page
        Page* page = bpm_->fetchPage(curr_page_id);

        //if it doesnt exist we'll have to create a new page
        if (page == nullptr) {
            curr_page_id = dm_->allocatePage();
        }


        HeapPage hp(page->get_data());
        //try and insert a tuple
        std::optional<int> slot_num = hp.insertTuple(to_insert);
        //if we couldnt fit go to our next page
        if (slot_num == std::nullopt) {
            curr_page_id = hp.getNextPage();
        } else {
            //otherwise we add it and are done!
            inserted = true;
            rid.slot_id = slot_num.value();
            rid.page_id = curr_page_id;
        }
    }


    return rid;
}

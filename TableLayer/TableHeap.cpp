//
// Created by Mark on 2/25/26.
//

#include "TableHeap.h"
#include "../RecordLayer/HeapPage.h"
#include "MemoryManagementLayer/BufferPoolManager.h"
#include "../Models/RID.h"


TableHeap::TableHeap(DiskManager* dm, BufferPoolManager *bpm): dm_(dm), bpm_(bpm) {
    first_page_id_ = dm_->allocatePage();
    Page* page = bpm_->newPage(first_page_id_);
    if (page != nullptr) {
        bpm_->unpinPage(first_page_id_, true);
        bpm_->flushPage(first_page_id_);
    }
}

TableHeap::TableHeap(DiskManager* dm, BufferPoolManager* bpm, int first_page_id)
    : dm_(dm), bpm_(bpm), first_page_id_(first_page_id) {
}


RID TableHeap::insertTuple(Tuple& to_insert) {
    int curr_page_id = first_page_id_;
    bool inserted = false;
    RID rid{-1,-1};
    while (!inserted) {
        //grab our page
        Page* page = bpm_->fetchPage(curr_page_id);

        //if it doesn't exist we'll have to create a new page
        if (page == nullptr) {
            curr_page_id = dm_->allocatePage();
        }


        HeapPage hp(page->get_data());
        //try and insert a tuple
        std::optional<int> slot_num = hp.insertTuple(to_insert);
        //if we couldnt fit go to our next page
        if (slot_num == std::nullopt) {
            bpm_->unpinPage(curr_page_id, false);
            curr_page_id = hp.getNextPage();
        } else {
            //otherwise we add it and are done!
            inserted = true;
            rid.slot_id = slot_num.value();
            rid.page_id = curr_page_id;
            bpm_->unpinPage(curr_page_id, true);
            bpm_->flushPage(curr_page_id);
        }
    }


    return rid;
}


bool TableHeap::getTuple(const RID &rid, Tuple& out) {
    int curr_page_id = first_page_id_;

    while (true) {
        Page* page = bpm_->fetchPage(curr_page_id);
        if (page == nullptr) {
            return false;
        }
        HeapPage hp(page->get_data());

        if (page->get_page_id() == rid.page_id) {
            out = hp.getTuple(rid.slot_id);
            bpm_->unpinPage(curr_page_id, false);
            return true;
        }
        bpm_->unpinPage(curr_page_id, false);
        curr_page_id = hp.getNextPage();
    }


}

std::vector<Tuple> TableHeap::scanTuples() {
    std::vector<Tuple> tuples;

    Page* page = bpm_->fetchPage(first_page_id_);
    if (page == nullptr) {
        return tuples;
    }

    HeapPage hp(page->get_data());
    for (std::uint16_t slot_id = 0; slot_id < hp.getNumSlots(); slot_id++) {
        Tuple tuple = hp.getTuple(slot_id);
        if (!tuple.data_.empty()) {
            tuples.push_back(std::move(tuple));
        }
    }

    bpm_->unpinPage(first_page_id_, false);
    return tuples;
}





bool TableHeap::applyDelete(const RID &rid) {
    int curr_page_id = first_page_id_;

    while (true) {
        Page* page = bpm_->fetchPage(curr_page_id);
        if (page == nullptr) {
            return false;
        }
        HeapPage hp(page->get_data());

        if (page->get_page_id() == rid.page_id) {

            hp.applyDelete(rid.slot_id);
            bpm_->unpinPage(curr_page_id, true);
            bpm_->flushPage(curr_page_id);
            return true;
        }
        bpm_->unpinPage(curr_page_id, false);
        curr_page_id = hp.getNextPage();
    }


}

bool TableHeap::editTuple(const RID &rid, Tuple &new_tuple) {
    int curr_page_id = first_page_id_;
    while (true) {
        Page* page = bpm_->fetchPage(curr_page_id);
        if (page == nullptr) {
            return false;
        }

        HeapPage hp(page->get_data());
        if (rid.page_id == curr_page_id) {

            //exit page accordingly
            if (!hp.changeTuple(rid.slot_id, new_tuple)) {
                //if we didnt have space to change the tuple we're going to want to delete it,
                //but then add it back in a page w/ space.
                hp.applyDelete(rid.slot_id);
                bpm_->unpinPage(curr_page_id, true);
                bpm_->flushPage(curr_page_id);
                this->insertTuple(new_tuple);
                return true;
            }
            bpm_->unpinPage(curr_page_id, true);
            bpm_->flushPage(curr_page_id);
            return true;
        }

        bpm_->unpinPage(curr_page_id, false);
        curr_page_id = hp.getNextPage();


    }
}

int TableHeap::getFirstPageId() const {
    return first_page_id_;
}


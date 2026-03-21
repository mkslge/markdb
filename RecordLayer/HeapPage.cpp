//
// Created by Mark on 2/24/26.
//

#include "HeapPage.h"
#include "../Models/Slot.h"
#include "../Models/HeapPageHeader.h"
#include "../RecordLayer/HeapPage.h"
#include <optional>
#include <vector>
#include "../Models/Page.h"
#include "../Models/Tuple.h"
#include<iostream>

HeapPage::HeapPage(char* data) {
    data_ = data;
    next_page_id_ = -1;
    prev_page_id_ = -1;
    HeapPageHeader* header = this->header();
    if (header->free_space_start == 0) {
        header->num_slots = 0;
        header->free_space_start = sizeof(HeapPageHeader);
        header->free_space_end = PAGE_SIZE;
    }
}

HeapPageHeader *HeapPage::header() {
    return reinterpret_cast<HeapPageHeader*>(data_);
}

std::uint16_t HeapPage::getNumSlots() {
    return header()->num_slots;
}

void  HeapPage::setNumSlots(int num_slots) {
    header()->num_slots = num_slots;
}


std::uint16_t HeapPage::getFreeSpace() {
    return header()->free_space_end - header()->free_space_start;
}

std::optional<int> HeapPage::insertTuple(Tuple &tuple) {
    int space_needed = tuple.size() + sizeof(Slot);
    //need to check how much space is needed

    if (header()->free_space_end - header()->free_space_start  < space_needed) {
        return std::nullopt;
    }

    //copy over data to page
    std::uint16_t start_of_tuple = header()->free_space_end - tuple.size();
    memcpy(data_ + start_of_tuple, tuple.data(), tuple.size());

    //update slot dir
    std::uint16_t slot_id = header()->num_slots;
    Slot* slot = reinterpret_cast<Slot*>(data_ + header()->free_space_start);
    header()->free_space_start += sizeof(Slot);
    slot->offset = start_of_tuple;
    slot->length = static_cast<int>(tuple.size());
    header()->num_slots++;

    //move free space pointer
    header()->free_space_end = start_of_tuple;

    return slot_id;
}

bool HeapPage::changeTuple(const std::uint16_t slot_id, Tuple &new_tuple) {
    if(slot_id < 0 || slot_id >= getNumSlots()) {
        return false;
    }
    
    Tuple curr_tuple = getTuple(slot_id);
    Slot* curr_slot = getSlot(slot_id);
    int new_size = new_tuple.size();
    int old_size = curr_slot->length;
    int size_diff = new_size - old_size;
    if(size_diff > getFreeSpace()) {
        return false;
    }

    if(new_size == old_size) {
        
        memcpy(data_ + curr_slot->offset, new_tuple.data(), new_size);
    } else if(new_size < old_size) {
        curr_slot->length = new_size;
        memcpy(data_ + curr_slot->offset, new_tuple.data(), new_size);
    } else {
        //here we actually need to shift.
        Slot* curr_slot = getSlot(slot_id );
        memmove(data_ + curr_slot->offset - size_diff, new_tuple.data(), new_tuple.size());

        curr_slot->length = new_size;
        curr_slot->offset -= size_diff;
        
        header()->free_space_start -= size_diff;
        
        for(int sid = slot_id + 1; sid < getNumSlots();sid++) {
            curr_slot = getSlot(sid);

            memmove(data_ + curr_slot->offset - size_diff, data_ + curr_slot->offset, curr_slot->length);
            curr_slot->offset -= size_diff;
        }

        //memmove(data_ + next_slot->offset + size_diff, data_ + next_slot->offset, next_slot->length);
        
        
        
    }

    return true;

}


Tuple HeapPage::getTuple(std::uint16_t slot_id) {
    Slot* slot = getSlot(slot_id);
    char* start = data_ +slot->offset;
    char* end = data_ + slot->offset + slot->length;

    //using constructor with start and end iterator points
    return {{start, end}};
}



void HeapPage::applyDelete(std::uint16_t slot_id) {
    Slot* slot = getSlot(slot_id);
    slot->length = 0;
}



Slot* HeapPage::getSlot(std::uint16_t slot_id) {
    return reinterpret_cast<Slot*>(data_ + sizeof(HeapPageHeader)) + slot_id;
}

void HeapPage::setSlot(std::uint16_t slot_id, const Slot &slot) {
    Slot* toReplace = getSlot(slot_id);
    toReplace->offset = slot.offset;
    toReplace->length = slot.length;
}

std::uint16_t HeapPage::getNextPage() const {
    return next_page_id_;
}

std::uint16_t HeapPage::getPrevPage() const {
    return prev_page_id_;
}

void HeapPage::setNextPage(std::uint16_t next_page) {
    next_page_id_ = next_page;
}

void HeapPage::setPrevPage(std::uint16_t prev_page) {
    prev_page_id_ = prev_page;
}







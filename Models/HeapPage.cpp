//
// Created by Mark on 2/24/26.
//

#include "HeapPage.h"

HeapPage::HeapPage(char* data) {
    data_ = data;
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
    return header()->free_space_start;
}

std::optional<int> HeapPage::insertTuple(const Tuple &tuple) {
    int space_needed = tuple.size() + sizeof(Slot);
    //need to check how much space is needed

    if (header()->free_space_end - header()->free_space_start  < space_needed) {
        std::cout << header()->free_space_end - header()->free_space_start << " " << space_needed << "\n";
        return std::nullopt;
    }
    //copy over data to page
    std::uint16_t start_of_tuple = header()->free_space_end - tuple.size();
    std::memcpy(data_ + start_of_tuple, tuple.data(), tuple.size());

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

Tuple HeapPage::getTuple(std::uint16_t slot_id) {
    Slot* slot = getSlot(slot_id);
    char* start = data_ +slot->offset;
    char* end = data_ + slot->offset + slot->length;

    //using constructor with start and end iterator points
    return {start, end};

}

void HeapPage::deleteTuple(std::uint16_t slot_id) {
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




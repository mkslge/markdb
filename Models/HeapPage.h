//
// Created by Mark on 2/24/26.
//

#ifndef HEAPPAGE_H
#define HEAPPAGE_H

#include "Slot.h"
#include "HeapPageHeader.h"
#include <optional>

using Tuple = std::vector<char>;


std::string example = "|ns: |";

class HeapPage {
private:
    char* data_;

public:
    HeapPage();
    HeapPageHeader* header();
    int getNumSlots();
    void setNumSlots(int num_slots);
    int getFreeSpace();

    std::optional<int> insertTuple(const Tuple& tuple);
    Tuple getTuple();

    void deleteTuple();


private:
    Slot getSlot(int slot_id);
    void setSlot(int slot_id, Slot slot)
};



#endif //HEAPPAGE_H

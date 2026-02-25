//
// Created by Mark on 2/24/26.
//

#ifndef HEAPPAGE_H
#define HEAPPAGE_H

#include "Slot.h"
#include "../Models/HeapPageHeader.h"
#include "../Models/HeapPage.h"
#include <optional>
#include <vector>
#include "Page.h"
#include<iostream>
using Tuple = std::vector<char>;

class HeapPage {
private:
    char* data_;

public:
    explicit HeapPage(char* data);
    HeapPageHeader* header();
    std::uint16_t getNumSlots();
    void setNumSlots(int num_slots);
    std::uint16_t getFreeSpace();

    std::optional<int> insertTuple(const Tuple& tuple);
    Tuple getTuple(std::uint16_t slot_id);
    void deleteTuple(std::uint16_t slot_id);


private:
    Slot* getSlot(std::uint16_t slot_id);
    void setSlot(std::uint16_t slot_id, const Slot& slot);
};



#endif //HEAPPAGE_H

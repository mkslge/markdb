//
// Created by Mark on 2/24/26.
//

#ifndef HEAPPAGE_H
#define HEAPPAGE_H

#include "../Models/Slot.h"
#include "../Models/HeapPageHeader.h"
#include "../RecordLayer/HeapPage.h"
#include <optional>
#include <vector>
#include "../Models/Page.h"
#include<iostream>
using Tuple = std::vector<char>;

class HeapPage {
private:
    char* data_;
    std::uint16_t next_page_id_;
    std::uint16_t prev_page_id_;

public:
    explicit HeapPage(char* data);
    HeapPageHeader* header();
    std::uint16_t getNumSlots();
    void setNumSlots(int num_slots);
    std::uint16_t getFreeSpace();

    std::optional<int> insertTuple(const Tuple& tuple);
    Tuple getTuple(std::uint16_t slot_id);
    void deleteTuple(std::uint16_t slot_id);

    [[nodiscard]] std::uint16_t getNextPage() const;
    [[nodiscard]] std::uint16_t getPrevPage() const;

    void setNextPage(std::uint16_t next_page);
    void setPrevPage(std::uint16_t prev_page);


private:
    Slot* getSlot(std::uint16_t slot_id);
    void setSlot(std::uint16_t slot_id, const Slot& slot);
};



#endif //HEAPPAGE_H

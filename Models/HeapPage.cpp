//
// Created by Mark on 2/24/26.
//

#include "HeapPage.h"

HeapPage::HeapPage(char* data) {
    data_ = data;
}

HeapPageHeader *HeapPage::header() {
    return reinterpret_cast<HeapPageHeader*>(data_);
}



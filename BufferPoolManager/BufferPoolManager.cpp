//
// Created by Mark on 2/19/26.
//

#include "BufferPoolManager.h"


BufferPoolManager::BufferPoolManager() {
    for (int i = 0; i < BUFFER_POOL_CAPACITY;i++) {
        unused_frames.push_back(i);
    }
}


Page *BufferPoolManager::fetchPage(int page_id) {
    //check if the bpm is in cache
        //if so return it and add a pin to it

    //otherwise try use a free frame
        //either take an unused one or evict an old page
        //update page table accordingly, mark pin as one, and dirty as false
}

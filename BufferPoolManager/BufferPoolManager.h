//
// Created by Mark on 2/19/26.
//

#ifndef BUFFERPOOLMANAGER_H
#define BUFFERPOOLMANAGER_H
#include <array>

#include "../DiskManager.h"
#include <map>
#include <list>
#include "Replacer/LRUReplacer.h"
#include "Replacer/Replacer.h"
constexpr int BUFFER_POOL_CAPACITY = 1000;

class BufferPoolManager {
private:
    DiskManager* dm_;

    std::array<Page, BUFFER_POOL_CAPACITY> frames_;

    //maps page_id to frame (index in frames_)
    std::unordered_map<int, int> frame_table_;

    //lists any frames that are unused
    std::list<int> unused_frames_;
public:
    LRUReplacer replacer_;
    BufferPoolManager();
    explicit BufferPoolManager(DiskManager* dm);
    Page* fetchPage(int page_id);
    bool unpinPage(int page_id, bool is_dirty);
    Page* newPage(int page_id);
    bool flushPage(int page_id);
    bool deletePage(int page_id);

private:



};



#endif //BUFFERPOOLMANAGER_H

//
// Created by Mark on 2/19/26.
//

#ifndef BUFFERPOOLMANAGER_H
#define BUFFERPOOLMANAGER_H
#include <array>

#include "../DiskManager.h"
#include <map>
#include <list>
constexpr int BUFFER_POOL_CAPACITY = 1000;

class BufferPoolManager {
private:
    DiskManager dm_;

    std::array<Page, BUFFER_POOL_CAPACITY> frames_;

    //maps page_id to frame (index in frames_)
    std::unordered_map<int, int> frame_table;

    //lists any frames that are unused
    std::list<int> unused_frames;
public:
    BufferPoolManager();
    Page* fetchPage(int page_id);
    bool unpinPage(int page_id, bool is_dirty);
    Page* newPage(int page_id);
    bool flushPage(int page_id);
    bool deletePage(int page_id);



};



#endif //BUFFERPOOLMANAGER_H

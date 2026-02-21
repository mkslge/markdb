//
// Created by Mark on 2/19/26.
//

#include "BufferPoolManager.h"


BufferPoolManager::BufferPoolManager()  {
    for (int i = 0; i < BUFFER_POOL_CAPACITY;i++) {
        unused_frames_.push_back(i);
    }
}

BufferPoolManager::BufferPoolManager(DiskManager* dm) {

    for (int i = 0; i < BUFFER_POOL_CAPACITY;i++) {
        unused_frames_.push_back(i);
    }
    dm_ = dm;
}


Page *BufferPoolManager::fetchPage(int page_id) {
    //check if the bpm is in cache
        //if so return it and add a pin to it
    if (frame_table_.contains(page_id)) {
        int frame_id = frame_table_[page_id];
        frames_[frame_id].pin();
        replacer_.pin(frame_id);

        return &frames_[frame_id];
    }
    //otherwise try use a free frame
    //either take an unused one or evict an old page
    //update page table accordingly, mark pin as one, and dirty as false
    int new_frame_idx = -1;
    Page* page = nullptr;
    if (!unused_frames_.empty()) {
        new_frame_idx = unused_frames_.front();
        unused_frames_.pop_front();
        page = &frames_[new_frame_idx];
    } else {

        if (!replacer_.victim(&new_frame_idx)) {
            return nullptr;
        } else {
            Page* victim = &frames_[new_frame_idx]; //need to write old page to disk if its dirty
            if (victim->is_dirty()) {
                dm_->writePage(victim->get_page_id(), victim->get_data());
            }
            //remove it from our lovely frame table after we saved it
            frame_table_.erase(victim->get_page_id());

            page = victim;
        }


    }
    //save new data
    page->set_page_id(page_id);
    page->set_dirty(false);
    page->set_pin_count(1);
    dm_->readPage(page->get_page_id(), page->get_data());

    frame_table_[page_id] = new_frame_idx;
    replacer_.pin(new_frame_idx);
    return page;
}

bool BufferPoolManager::unpinPage(int page_id, bool is_dirty) {
    if (!frame_table_.contains(page_id)) {
        return false;
    }
    int frame_id = frame_table_[page_id];
    Page* page = &frames_[frame_id];
    if (page->get_pin_count() == 0) {
        return false;
    }
    frames_[frame_id].unpin();
    if (is_dirty) {
        frames_[frame_id].set_dirty(is_dirty);
    }
    if (frames_[frame_id].get_pin_count() == 0) {
        replacer_.unpin(frame_id);
    }

    return true;
}

Page* BufferPoolManager::newPage(int page_id) {

    int new_frame_id = -1;
    if (unused_frames_.empty()) {
        int victim_frame_id;

        if (!replacer_.victim(&victim_frame_id)) {
            return nullptr;
        }
        Page* victim = &frames_[victim_frame_id];

        if (victim->is_dirty()) {
            dm_->writePage(victim->get_page_id(), victim->get_data());
        }
        frame_table_.erase(victim->get_page_id());
        new_frame_id = victim_frame_id;
    } else {
        new_frame_id = unused_frames_.front();
        unused_frames_.pop_front();
    }

    int new_page_id = static_cast<size_t>(dm_->allocatePage());
    frame_table_[new_page_id] = new_frame_id;
    frames_[new_frame_id] = Page();
    frames_[new_frame_id].set_page_id(new_page_id);
    frames_[new_frame_id].set_dirty(true);
    frames_[new_frame_id].set_pin_count(1);
    return &frames_[new_frame_id];
}

bool BufferPoolManager::flushPage(int page_id) {
    if (!frame_table_.contains(page_id)) {
        return false;
    }
    int frame_id = frame_table_[page_id];
    Page* page = &frames_[frame_id];
    dm_->writePage(page->get_page_id(), page->get_data());
    page->set_dirty(false);
    return true;
}

bool BufferPoolManager::deletePage(int page_id) {
    if (!frame_table_.contains(page_id)) {
        return false;
    }
    int frame_id = frame_table_[page_id];
    Page* page = &frames_[frame_id];

    if (page->get_pin_count() != 0) {
        return false;
    }

    frame_table_.erase(page_id);
    frames_[frame_id].reset();
    unused_frames_.push_front(frame_id);
    return true;
}






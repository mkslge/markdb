//
// Created by Mark on 2/11/26.
//

#include "Page.h"


Page::Page() : page_id_(INVALID_PAGE_ID),  is_dirty_(false), pins_(0){
    memset(data_, 0, 4096);
}

int Page::get_page_id() const {
    return page_id_;
}

void Page::set_page_id(int id) {
    page_id_ = id;
}

bool Page::is_dirty() const {
    return is_dirty_;
}

void Page::set_dirty(bool dirty) {
    is_dirty_ = dirty;
}

int Page::get_pin_count() {
    return pins_;
}

void Page::pin() {
    pins_++;
}

void Page::unpin() {
    if (pins_ > 0) {
        pins_--;
    }
}

char *Page::get_data() {
    return data_;
}

const char *Page::get_data() const {
    return data_;
}

void Page::reset() {
    reset_memory();
    reset_metadata();
}


void Page::reset_memory() {
    memset(data_, 0, 4096);
}

void Page::reset_metadata() {
    page_id_ = INVALID_PAGE_ID;
    is_dirty_ = false;
    pins_ = 0;
}
















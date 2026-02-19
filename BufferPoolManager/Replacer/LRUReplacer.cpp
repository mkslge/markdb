//
// Created by Mark on 2/19/26.
//

#include "LRUReplacer.h"

LRUReplacer::LRUReplacer() : dummy_start_(new Node(0)), dummy_end_(new Node(0)){
    dummy_start_->next_ = dummy_end_;
    dummy_end_->prev_ = dummy_start_;
}

bool LRUReplacer::victim(int* frame_id) {
    if (map_.empty()) {
        return false;
    }
    Node* lru_frame = dummy_start_->next_()->key_;
    deleteNode(lru_frame);
    map_.erase(lru_frame->key_);
    *frame_id = lru_frame->key_;

    return true;
}

void LRUReplacer::pin(int frame_id) {
    if (map_.contains(frame_id)) {
        Node* to_remove = map_[frame_id];
        deleteNode(to_remove);
        map_.erase(frame_id);
    }
}

void LRUReplacer::unpin(int frame_id) {
    if (!map_.contains(frame_id)) {
        Node* newNode = new Node(frame_id);
        addToBack(newNode);
        map_[frame_id] = newNode;
    }
}

size_t LRUReplacer::size() {
    return std::size(map_);
}

void LRUReplacer::deleteNode(Node *to_delete) {
    to_delete->prev_->next_ = to_delete->next_;
    to_delete->next_->prev_ = to_delete->prev_;
}

void LRUReplacer::addToBack(Node* to_move) {

    dummy_end_->prev_->next_ = to_move;
    to_move->prev_ = dummy_end_->prev_;

    dummy_end_->prev_ = to_move;
    to_move->next_ = dummy_end_;
}

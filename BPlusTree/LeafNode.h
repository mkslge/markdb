#ifndef LEAFNODE_H      
#define LEAFNODE_H

#include "Node.h"
#include <algorithm>
#include <cassert>
#include <vector>

template <typename T>
class LeafNode : public Node<T> {
private:
    LeafNode* prev_;
    LeafNode* next_;
    std::vector<T> keys_;

public:
    LeafNode() : prev_(nullptr), next_(nullptr) {}

    /*
     * Preconditions:
     * - `keys_` is already sorted in ascending order.
     *
     * Postconditions:
     * - `val` is inserted into `keys_` in sorted order.
     * - The return value is the index where `val` was inserted.
     */
    int add(const T& val) override {
        for (std::size_t i{}; i < keys_.size(); i++) {
            if (val < keys_[i]) {
                keys_.insert(keys_.begin() + static_cast<long>(i), val);
                return static_cast<int>(i);
            }
        }

        keys_.push_back(val);
        return static_cast<int>(keys_.size() - 1);
    }

    LeafNode<T>* prev() {
        return prev_;
    }

    LeafNode<T>* next() {
        return next_;
    }

    void setPrev(LeafNode<T>* prev) {
        prev_ = prev;
    }

    void setNext(LeafNode<T>* next) {
        next_ = next;
    }

    T at(int index) override {
        assert(index >= 0 && index < static_cast<int>(keys_.size()));
        return keys_[index];
    }

    T get(int index) {
        return at(index);
    }

    bool isLeaf() override {
        return true;
    }

    std::size_t size() override {
        return keys_.size();
    }

    bool contains(const T& val) override {
        return std::find(keys_.begin(), keys_.end(), val) != keys_.end();
    }

    std::string toString() override {
        std::string builder;
        for (const T& key : keys_) {
            builder.append(std::to_string(key) + " ");
        }
        return builder;
    }

    std::vector<T> getList() override {
        return keys_;
    }

    void setKeys(const std::vector<T>& keys) {
        keys_ = keys;
    }

    int getIndex(const T& val) override {
        for (std::size_t i{}; i < keys_.size(); i++) {
            if (val < keys_[i]) {
                return static_cast<int>(i);
            }
        }

        return keys_.empty() ? 0 : static_cast<int>(keys_.size() - 1);
    }

    /*
     * Preconditions:
     * - None.
     *
     * Postconditions:
     * - Removes the first matching occurrence of `val` if present.
     * - Returns `true` if a key was erased, otherwise `false`.
     */
    bool remove(const T& val) {
        for (std::size_t i{}; i < keys_.size(); i++) {
            if (keys_[i] == val) {
                keys_.erase(keys_.begin() + static_cast<long>(i));
                return true;
            }
        }

        return false;
    }

};

#endif

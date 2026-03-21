

#ifndef LEAFNODE_H      
#define LEAFNODE_H

#include <algorithm>
#include <cassert>
#include "Node.h"
#include <vector>

template <typename T>
class LeafNode : public Node<T> {
    private:
        LeafNode* prev_;
        LeafNode* next_;
        std::vector<T> keys_;

    public:
        LeafNode() {};

        int add(const T& val) {
            for(size_t i{}; i < keys_.size();i++) {
                if(val < keys_[i]) {
                    keys_.insert(keys_.begin() + i, val);
                    return i;
                }
            }
            keys_.push_back(val);
            return keys_.size() - 1;
        }

        T at(const int index) {
            assert(index >= 0 && index < static_cast<int>(keys_.size()));
            return keys_[index];
        }

        bool isLeaf() {
            return true;
        }

        size_t size() {
            return keys_.size();
        }

        bool contains(const T& val) {
            return std::find(keys_.begin(), keys_.end(), val) != keys_.end();
        }

        std::string toString() {
            std::string builder = "";
            for(T& key : keys_) {
                builder.append(std::to_string(key) + " ");
            }
            return builder;
        }

        std::vector<T> getList() {
            return this->keys_;
        }

        void setKeys(const std::vector<T>& keys) {
            keys_ = keys;
        }
    

};

#endif

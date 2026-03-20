

#ifndef LEAFNODE_H      
#define LEAFNODE_H

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

        void add(const T& val) {
            keys_.emplace_back(val);
        }

        T at(const int index) {
            assert(index >= 0 || index < keys_.size());
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

    

};

#endif
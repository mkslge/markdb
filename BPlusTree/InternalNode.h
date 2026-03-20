#ifndef INTERNALNODE_H
#define INTERNALNODE_H

#include <vector>
#include "Node.h"

template <typename T>
class InternalNode : public Node<T> {
    private:
        std::vector<T> values_;
        std::vector<Node<T>*> children_;

    public:
        InternalNode() {};
        void add(const T& val) {
            values_.emplace_back(val);
        }

        T at(const int index) {
            assert(index >= 0 || index < values_.size());
            return values_[index];
        }

        void insertChild(Node<T>* child, int index) {

            children_.insert(child, index);
        }

        bool removeChild(Node<T>* child) {
            for(size_t i{}; i < children_.size();i++) {
                if(child == children_[i]) {
                    children_.erase(children_.begin() + i);
                    return true;
                }
            }
            return false;
        }

        Node<T>* childAt(int index) {
            assert(index >= 0|| index < children_.size() );
            return children_[index];
        }

        

        bool isLeaf() {
            return false;
        }

        bool contains(const T& val) {
            return std::find(values_.begin(), values_.end(), val) != values_.end();
        }

        size_t size() { return values_.size(); };

        size_t children_size() {return children_.size(); };

        




};




#endif //INTERNALNODE_H
#ifndef INTERNALNODE_H
#define INTERNALNODE_H

#include <vector>
#include <array>

template <typename T>
class InternalNode : public Node {
    std::array<T> sections;
    std::array<Node> children;
     
};




#endif //INTERNALNODE_H
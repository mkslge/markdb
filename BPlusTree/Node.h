


#ifndef NODE_H
#define NODE_H


template <typename T>
class Node {
    public:
        virtual ~Node() = default;

        virtual T at(const int index) = 0;
        virtual void add(const T& val) = 0;
        virtual bool isLeaf() = 0;
        virtual size_t size() = 0;
        virtual bool contains(const T& val) = 0;
};

#endif // NODE_H
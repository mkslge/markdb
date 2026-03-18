//
// Created by Mark on 2/24/26.
//

#ifndef TUPLE_H
#define TUPLE_H

#include <vector>

class Tuple {
public:
    std::vector<char> data_;
    std::size_t size() const {
        return data_.size(); 
    }

    char* data() {
        return data_.data();
    }
};



#endif //TUPLE_H

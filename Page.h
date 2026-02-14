//
// Created by Mark on 2/11/26.
//

#ifndef PAGE_H
#define PAGE_H

#include <string>
inline std::size_t PAGE_SIZE = 4096;

class Page {
public:


private:
    int index;
    int rows;
    int curr_size;
public:
    Page();
    ~Page();

};



#endif //PAGE_H

//
// Created by Mark on 2/24/26.
//

#ifndef HEAPPAGEHEADER_H
#define HEAPPAGEHEADER_H

#include <cstdint>



#pragma pack(1)

struct HeapPageHeader {
    std::uint16_t num_slots;
    std::uint16_t free_space_start;
    std::uint16_t free_space_end;
};

#endif //HEAPPAGEHEADER_H

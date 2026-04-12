//
// Created by Mark on 2/24/26.
//

#ifndef HEAPPAGEHEADER_H
#define HEAPPAGEHEADER_H

#include <cstdint>

#pragma pack(push, 1)

struct HeapPageHeader {
    std::uint32_t num_slots;
    std::uint32_t free_space_start;
    std::uint32_t free_space_end;
};

#pragma pack(pop)

#endif //HEAPPAGEHEADER_H

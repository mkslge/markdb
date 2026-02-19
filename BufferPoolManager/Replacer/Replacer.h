#ifndef REPLACER_H
#define REPLACER_H

#include <cstddef>

class Replacer {
public:
    virtual ~Replacer() = default;

    virtual bool victim(int* frame_id) = 0;
    virtual void pin(int frame_id) = 0;
    virtual void unpin(int frame_id) = 0;
    virtual std::size_t size() const = 0;
};

#endif
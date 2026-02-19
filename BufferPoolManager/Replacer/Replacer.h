//
// Created by Mark on 2/19/26.
//

#ifndef REPLACER_H
#define REPLACER_H



class Replacer {
public:


    // @out: frame_id to be returned via frame_id
    virtual bool victim(int* frame_id);

    virtual void pin(int frame_id);
    virtual void unpin(int frame_id);
    virtual size_t size() = 0;
};



#endif //REPLACER_H

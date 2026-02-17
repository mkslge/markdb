//
// Created by Mark on 2/11/26.
//

#ifndef PAGE_H
#define PAGE_H

#include <string>
constexpr std::size_t PAGE_SIZE = 4096;
inline int INVALID_PAGE_ID = -1;

class Page {
public:


private:
    int page_id_;
    char data_[PAGE_SIZE];
    bool is_dirty_;
    int pins_;
public:
    Page();

    [[nodiscard]] int get_page_id() const;
    void set_page_id(int id);

    [[nodiscard]] bool is_dirty() const;
    void set_dirty(bool dirty);

    [[nodiscard]] int get_pin_count();
    void pin();
    void unpin();

    [[nodiscard]] const char* get_data() const;
    [[nodiscard]] char* get_data();

    void reset_memory();
    void reset_metadata();
    void reset();





};



#endif //PAGE_H

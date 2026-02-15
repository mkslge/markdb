//
// Created by Mark on 2/11/26.
//

#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include <iosfwd>
#include <string>

#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include "Page.h"

inline std::string filepath = "/Users/markseeliger/CLionProjects/marksql/Storage/temp.db";
//0644 is the file permissions that grants the owner read and write, and everyone else read only
inline int filePermissionCode = 0666;

class DiskManager {

private:
    std::string filepath_;
    int fd_;
    size_t next_page_id_;
public:
    DiskManager();
    explicit DiskManager(std::string customFilepath);
    char* readPage(int page_id, char* buf);
    void writePage(int page_id, char* buf);

    size_t allocatePage();

    static off_t getByteOffset(size_t page_id);
    ~DiskManager();

private:

};



#endif //DISKMANAGER_H

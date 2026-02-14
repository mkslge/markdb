//
// Created by Mark on 2/11/26.
//

#include "DiskManager.h"

#include <utility>


DiskManager::DiskManager() : DiskManager(filepath) {
}



DiskManager::DiskManager(std::string customFilePath) : filepath_(std::move(customFilePath)){

    fd_ = open(filepath_.c_str(), O_RDWR | O_CREAT, filePermissionCode);

    if (fd_ < 0) {
        perror("open");
        throw std::runtime_error("File opening failed.");
    }  else {
        std::cout << "Opened file." << std::endl;
    }
}



DiskManager::~DiskManager() {
    if (fd_ >= 0) {
        close(fd_);
    }
}



void DiskManager::writePage(int page_id, char *buf) {
    off_t start = DiskManager::getByteOffset(page_id);
    ssize_t status = pwrite(fd_, buf, PAGE_SIZE, start);

    if (status != PAGE_SIZE) {
        throw std::runtime_error("Error in writePage()");
    }

}


char* DiskManager::readPage(int page_id, char* buf) {

    off_t start = DiskManager::getByteOffset(page_id);

    //read page from offset, read of size PAGE_SIZE;
    int status = pread(fd_, buf, PAGE_SIZE, start);

    if (status < 0) {
        throw std::runtime_error("Error reading page");
    }



    return buf;
}



 off_t DiskManager::getByteOffset(size_t page_id) {

    return static_cast<off_t>(page_id) * PAGE_SIZE;
}
